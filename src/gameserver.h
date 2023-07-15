#ifndef KINGOFTHEHILL_KI_GAMESERVER_H
#define KINGOFTHEHILL_KI_GAMESERVER_H


#include <string>
#include <utility>
#include <vector>
#include "lib/json.hpp"
using json = nlohmann::json;

enum RequestType {
    LOGIN = 0,

    GET_GAMES = 1,
    CREATE_GAME = 2,
    JOIN_GAME = 3,

    MOVE = 4,
    DATABASE = 5,

    START_TOURNAMENT = 6,
    GET_PLAYERS = 7,
    GAME_STARTED = 8,

    ERROR = -1,
    ILLEGAL_MOVE = -2,
    NOT_IMPLEMENTED = -3,
    INVALID_REQUEST = -4,
    UNAUTHORIZED = -5,
    FULL = -6,
    NOT_FOUND = -7
};

class Request {
public:
    virtual std::string toJson() = 0;
};

class Response {
};


std::string stringRequestType(RequestType type) {
    switch (type) {
        case LOGIN:
            return "LOGIN";
        case GET_GAMES:
            return "GET_GAMES";
        case CREATE_GAME:
            return "CREATE_GAME";
        case JOIN_GAME:
            return "JOIN_GAME";
        case MOVE:
            return "MOVE";
        case DATABASE:
            return "DATABASE";
        case START_TOURNAMENT:
            return "START_TOURNAMENT";
        case GET_PLAYERS:
            return "GET_PLAYERS";
        case GAME_STARTED:
            return "GAME_STARTED";
        case ERROR:
            return "ERROR";
        case ILLEGAL_MOVE:
            return "ILLEGAL_MOVE";
        case NOT_IMPLEMENTED:
            return "NOT_IMPLEMENTED";
        case INVALID_REQUEST:
            return "INVALID_REQUEST";
        case UNAUTHORIZED:
            return "UNAUTHORIZED";
        case FULL:
            return "FULL";
        case NOT_FOUND:
            return "NOT_FOUND";
    }
}


class Player: public Response {
public:
    std::string playerName;
    int playerID;
    int rating = 0;

    explicit Player(const json& data) {
        playerName = data["playerName"];
        playerID = data["playerID"];
        rating = data["rating"];
    }

    explicit Player(const std::string& jsonData) {
        json data = json::parse(jsonData);

        playerName = data["playerName"];
        playerID = data["playerID"];
        rating = data["rating"];
    }

    Player(std::string name, int id) {
        playerName = std::move(name);
        playerID = id;
    }
};


class GameInfo: public Response {
public:
    int gameID;
    std::vector<Player> activePlayerList;
    int maxPlayerNumber = 2;

    Player currentPlayer = Player("", -1);
    std::string FEN;

    std::pair<float, float> timeLeft;
    std::vector<float> moveTimestamps;
    std::vector<std::string> moveHistory;

    bool isOver;
    bool isDraw;
    bool isCheck;
    bool isRepetitionDraw;
    bool isInsufficientMaterial;
    bool isStalemate;
    bool isCheckMate;

    Player winner = Player("", -1);

    explicit GameInfo(const std::string& jsonData) {
        json data = json::parse(jsonData);

        gameID = data["gameID"];

        activePlayerList = std::vector<Player>();
        for (const auto& player: data["activePlayerList"]) {
            activePlayerList.emplace_back(player);
        }

        maxPlayerNumber = data["maxPlayerNumber"];

        currentPlayer = Player(data["currentPlayer"]);
        FEN = data["fen"];

        timeLeft = std::pair<float, float>(data["timeLeft"]);
        moveTimestamps = std::vector<float>(data["moveTimestamps"]);
        moveHistory = std::vector<std::string>(data["moveHistory"]);

        isOver = data["over"];
        isDraw = data["draw"];
        isCheck = data["check"];
        isRepetitionDraw = data["inThreeFoldRepetition"];
        isInsufficientMaterial = data["insufficientMaterial"];
        isStalemate = data["stalemate"];
        isCheckMate = data["checkMate"];

        winner = Player(data["winner"]);
    }
};


/// REQUEST CLASSES
class LoginRequest: public Request {
public:
    RequestType type = RequestType::LOGIN;
    int messageStamp = 0;

    std::string username;
    int playerID = -1;

    explicit LoginRequest(std::string uname) {
        username = std::move(uname);
    }

    std::string toJson() override {
        json data = {
                {"type", type},
                {"stamp", messageStamp},
                {"username", username},
                {"playerID", playerID}
        };

        return to_string(data);
    }
};
typedef Player LoginResponse;


class GetGamesRequest: public Request {
    RequestType type = RequestType::GET_GAMES;
    int messageStamp = 0;

    std::string toJson() override {
        json data = {
                {"type", type},
                {"stamp", messageStamp},
        };

        return to_string(data);
    }
};
class GetGamesResponse {
public:
    std::vector<GameInfo> games;

    explicit GetGamesResponse(const json& data) {
        games = std::vector<GameInfo>();
        for (const auto& game : data["games"]) {
            games.emplace_back(game);
        }
    }
};


class CreateGameRequest: public Request{
    RequestType type = RequestType::CREATE_GAME;
    int messageStamp = 0;

    std::string username;
    int playerID;
    int timeout;
    int gameRetentionTime;

    CreateGameRequest(std::string uname, int pid, int timeout, int retentionTime) {
        username = std::move(uname);
        playerID = pid;
        this->timeout = timeout;
        gameRetentionTime = retentionTime;
    }

    json data = {
            {"type", type},
            {"stamp", messageStamp},
            {"username", username},
            {"playerID", playerID},
            {"timeout", timeout},
            {"gameRetentionTime", gameRetentionTime}
    };
};
typedef GameInfo CreateGameResponse;


enum JoinType {
    PLAYER = 1
};
class JoinGameRequest: public Request {
    RequestType type = RequestType::JOIN_GAME;
    int messageStamp = 0;

    std::string username;
    int playerID;
    JoinType joinAsPlayer;
    int gameID;

    JoinGameRequest(std::string uname, int pid, JoinType asPlayer, int gid) {
        username = std::move(uname);
        playerID = pid;
        joinAsPlayer = asPlayer;
        gameID = gid;
    }

    std::string toJson() override {
        json data = {
                {"type", type},
                {"stamp", messageStamp},
                {"username", username},
                {"playerID", playerID},
                {"joinAsPlayer", joinAsPlayer},
                {"gameID", gameID}
        };

        return to_string(data);
    }
};
typedef GameInfo JoinGameResponse;


class MoveRequest: public Request {
    RequestType type = RequestType::MOVE;
    int messageStamp = 0;

    std::string username;
    int playerID;
    int gameID;
    std::string move;

    MoveRequest(std::string uname, int pid, int gid, std::string move) {
        username = std::move(uname);
        playerID = pid;
        gameID = gid;
        this->move = std::move(move);
    }

    std::string toJson() override {
        json data = {
                {"type", type},
                {"stamp", messageStamp},
                {"username", username},
                {"playerID", playerID},
                {"gameID", gameID},
                {"move", move}
        };

        return to_string(data);
    }
};
typedef GameInfo MoveResponse;


class StartTournamentRequest: public Request {
    RequestType type = RequestType::START_TOURNAMENT;
    int messageStamp = 0;

    int ticks;
    int gameTimeout;
    int gameRetentionTime;

    StartTournamentRequest(int ticks, int gameTimeout, int retentionTime) {
        this->ticks = ticks;
        this->gameTimeout = gameTimeout;
        this->gameRetentionTime = retentionTime;
    }

    std::string toJson() override {
        json data = {
                {"type", type},
                {"stamp", messageStamp},
                {"ticks", ticks},
                {"gameTimeout", gameTimeout},
                {"gameRetentionTime", gameRetentionTime}
        };

        return to_string(data);
    }
};
typedef Response StartTournamentResponse;


// GameStarted is exclusively sent by the server
typedef Response GameStartedResponse;


int setup_connection(const std::string& address, int port);
void sendRequest(int fd, Request *request);
Response *awaitReceive(int fd);

#endif //KINGOFTHEHILL_KI_GAMESERVER_H
