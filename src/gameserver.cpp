#include "gameserver.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <hikaru.h>


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

void playGameserver(uint64_t gameTime) {
    std::basic_string<char> address = "127.0.0.1";
    int port = 8025;
    std::basic_string<char> username = "Hikaru";
    int ticks = 10;
    int timeout = 50000;
    int retentionTime = 500;
    t_game game = t_game(gameTime);
    bool color;

    //create connection
    int client_fd = setup_connection(address, port);

    //login to server
    LoginRequest loginRQ = LoginRequest(username);
    sendRequest(client_fd,&loginRQ);
    Player* player = (Player*) awaitReceive(client_fd);
    if(player == nullptr) {
        printf("Login failed!");
    }

    //start Tournament
    StartTournamentRequest startTour = StartTournamentRequest(ticks,timeout,retentionTime);
    sendRequest(client_fd, &startTour);
    awaitReceive(client_fd); //need error handling here

    //Start Game
    CreateGameRequest srvGame = CreateGameRequest(username,player->playerID,timeout,retentionTime);
    GameInfo* gameState =(GameInfo*) awaitReceive(client_fd);
    if(gameState == nullptr){
        printf("Start game failed!");
        return;
    }
    if(gameState->currentPlayer.playerID == player->playerID){
        color=false;
        game.turn=false;
    }else{
        color=true;
        game.turn=false;
    }
    
    //update local game object
    updateGame(gameState,&game,player,color);
    printBoard(game.state->board);

    // Generate next move
    t_gameState *nextMove = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
    while (!game.isOver) {
        std::chrono::nanoseconds diff;
        if (game.turn == color) {
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            std::pair<t_gameState, float> result = getMoveAlphaBeta<true>(&game);
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            memcpy(nextMove, &result.first, sizeof(t_gameState));

            //do move
            printf("Send move ");
            printMove(*nextMove, ' ');
            MoveRequest mvRQ = MoveRequest(username,player->playerID,gameState->gameID, moveToString(*nextMove));

            //wait for acceptance of own move
            gameState =(GameInfo*) awaitReceive(client_fd);
            if(gameState == nullptr){
                printf("Move failed");
                continue;
            }else {
                updateGame(gameState, &game, player, color);
            }

            //wait for opponents move
            gameState =(GameInfo*) awaitReceive(client_fd);
            if(gameState == nullptr){
                printf("Error during move of opponent");
                continue;
            }else {
                updateGame(gameState, &game, player, color);
            }
        }

        // Print game state information
        printf("Current board state (Score: %.4f, Round: %d, ", evaluate(&game), game.moveCounter/2 + 1);

        if (gameState->isCheck) {
            printf("CHECK\n");
        }

        // Print board
        printBoard(game.board());

    }
    free(nextMove);

    printf("Game over!\n");
    printf("Game is over: %d\n", game.isOver);
    printf("White won: %d\n", game.whiteWon);
    printf("Black won: %d\n", game.blackWon);
    printf("Total rounds: %d\n", game.moveCounter/2 + 1);
    printf("Turn: %d\n", game.turn);

    // Free memory
    delete game.positionHistory;
    free(game.random);
    free(game.state);
}

int updateGame(GameInfo* gameInfo, t_game* game, Player* player,bool color){
    if(gameInfo->currentPlayer.playerID == player->playerID){
        game->turn=0; //have to find out how color is codet in data
        gameState* gs = new  gameState(setFen((char*) gameInfo->FEN.data()),game->state->wCastleShort,game->state->wCastleLong,game->state->bCastleShort,game->state->bCastleLong,game->state->enpassant);
        delete(game->state);
        game->state = gs;
        game->isOver = gameInfo->isOver;
        if(gameInfo->isOver&&!(gameInfo->isDraw||gameInfo->isStalemate)){
            game->whiteWon = gameInfo->currentPlayer.playerID == player->playerID ? (color==false ? true : false) : (color==false ? false : true);
            game->blackWon = !game->whiteWon;
        }
    }
}
