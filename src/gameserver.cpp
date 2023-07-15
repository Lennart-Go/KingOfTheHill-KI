#include "gameserver.h"
#include <netinet/in.h>
#include <arpa/inet.h>


int setup_connection(const std::string &address, int port) {
    struct sockaddr_in server_address;

    int client_fd;
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Could not create socket\n");
        throw 4321;  // TODO: Throw proper error
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((short) port);
    inet_pton(AF_INET, address.c_str(), &server_address);

    int status = connect(client_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (status < 0) {
        printf("Connection failed\n");
        throw 1234;  // TODO: Throw proper error
    }

    return client_fd;
}


void sendRequest(int fd, Request *request) {
    std::string requestString = request->toJson();
    char *sendBuffer = const_cast<char *>(requestString.c_str());

    size_t nr_sent;
    while (true) {
        nr_sent = send(fd, sendBuffer, strlen(sendBuffer), 0);

        if (nr_sent < strlen(sendBuffer)) {
            sendBuffer += nr_sent * sizeof(char);
        } else {
            break;
        }
    }
}


Response *awaitReceive(int fd) {
    char buffer[4096];
    read(fd, buffer, 4096);

    json jsonData = json::parse(buffer);
    switch ((RequestType) jsonData["type"]) {
        case LOGIN:
            return reinterpret_cast<Response *>(new LoginResponse(jsonData));
        case GET_GAMES:
            return reinterpret_cast<Response *>(new GetGamesResponse(jsonData));
        case CREATE_GAME:
            return reinterpret_cast<Response *>(new CreateGameResponse(jsonData));
        case JOIN_GAME:
            return reinterpret_cast<Response *>(new JoinGameResponse(jsonData));
        case MOVE:
            return reinterpret_cast<Response *>(new MoveResponse(jsonData));
        case DATABASE:
            return nullptr;
        case START_TOURNAMENT:
            return reinterpret_cast<Response *>(new StartTournamentResponse());
        case GET_PLAYERS:
            return nullptr;
        case GAME_STARTED:
            return reinterpret_cast<Response *>(new GameStartedResponse());
        default:
            return nullptr;
    }
}
