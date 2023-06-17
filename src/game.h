#include <map>
#include <stack>

#include "util.h"
#include "board.h"
#include "move.h"

#ifndef KINGOFTHEHILL_KI_GAME_H
#define KINGOFTHEHILL_KI_GAME_H



typedef struct gameOld {
    t_board *board;

    uint64_t gameTime;
    unsigned turn:1;  // 0 for white, 1 for black
    uint64_t latestMoveTime;
    bool isOver;
    std::map<std::string,int> *positionHistory;

    unsigned whiteWon:1;
    unsigned whiteCanCastleShort:1;
    unsigned whiteCanCastleLong:1;
    unsigned whiteCastled:1;
    uint32_t whiteMoveCounter;
    uint32_t whiteMoveTime;  // Cumulative move time of white team in ms

    unsigned blackWon:1;
    unsigned blackCanCastleShort:1;
    unsigned blackCanCastleLong:1;
    unsigned blackCastled:1;
    uint32_t blackMoveCounter;
    uint32_t blackMoveTime;  // Cumulative move time of black team in ms

    unsigned enpassants:4;  // 0 for no enpassant, otherwise the number of the file (1 -> A, 2 -> B, ...)
} t_gameOld;

typedef struct game {
    t_gameState *state;
    std::stack<t_gameState> stateStack;

    std::map<std::string, int> *positionHistory = nullptr;

    bool turn;
    uint64_t gameTime;

    bool isOver;
    bool whiteWon;
    bool blackWon;

    short moveCounter;

    explicit game(uint64_t time) {
        // Default game constructor
        t_board startBoard = setFen((char *)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
        t_move zeroMove = t_move();

        t_gameState startState = t_gameState(startBoard, zeroMove);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        turn = false;
        gameTime = time;

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;
    }
    game(char *startFen, bool color, uint64_t time) {
        // Game constructor from FEN and side to move
        t_board startBoard = setFen(startFen);
        t_move zeroMove = t_move();

        t_gameState startState = t_gameState(startBoard, zeroMove);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        turn = color;
        gameTime = time;

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;
    }
    game(char *startFen, bool color, uint8_t castleCode, uint64_t time) {
        // Game constructor from FEN, side to move and castling code
        t_board startBoard = setFen(startFen);
        t_move zeroMove = t_move();

        bool wCastleShort = (castleCode & 0b0001) != 0;
        bool wCastleLong = (castleCode & 0b0010) != 0;
        bool bCastleShort = (castleCode & 0b0100) != 0;
        bool bCastleLong = (castleCode & 0b1000) != 0;

        t_gameState startState = t_gameState(startBoard, zeroMove,
                                             wCastleShort, wCastleLong,
                                             bCastleShort, bCastleLong,
                                             0);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        turn = color;
        gameTime = time;

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;
    }
    game(char *startFen, bool color, uint8_t castleCode, uint8_t ep, uint64_t time) {
        // Game constructor from FEN, side to move and castling code
        t_board startBoard = setFen(startFen);
        t_move zeroMove = t_move();

        bool wCastleShort = (castleCode & 0b0001) != 0;
        bool wCastleLong = (castleCode & 0b0010) != 0;
        bool bCastleShort = (castleCode & 0b0100) != 0;
        bool bCastleLong = (castleCode & 0b1000) != 0;

        t_gameState startState = t_gameState(startBoard, zeroMove,
                                             wCastleShort, wCastleLong,
                                             bCastleShort, bCastleLong,
                                             ep);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        turn = color;
        gameTime = time;

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;
    }

    void positionTracking() {
        /* Function to check if same position occurred for the third time
        * Arguments:
        *  t_gameOld *gameOld: Pointer to the gameOld representing the state of the gameOld
        */
        std::map<std::string, int> &map = *positionHistory;
        char *currentFen = FEN();

        if (positionHistory == nullptr) {
            std::map<std::string, int> *mapInit = new std::map<std::string, int>();
            positionHistory = mapInit;

            positionHistory->insert(std::make_pair(currentFen, 1));
        } else {
            if (map.find(currentFen) != map.end()) {
                int n = map[currentFen];
                map[currentFen] = n + 1;
            } else {
                map.insert(std::make_pair(currentFen, 1));
            }
        }

        free(currentFen);
    }

    void positionTrackingUndo() const {
        std::map<std::string, int> &map = *positionHistory;
        char *currentFen = FEN();

        if (positionHistory != nullptr) {
            if (map.find(currentFen) != map.end()) {
                int n = map[currentFen];

                if (n > 0) {
                    map[currentFen] = n - 1;
                }
            }
        }

        free(currentFen);
    }

    int positionRepetitions() const {
        std::map<std::string, int> &map = *positionHistory;
        if (positionHistory != nullptr) {
            char *currentFen = FEN();

            if (map.find(currentFen) != map.end()) {
                int reps = map[currentFen];

                free(currentFen);
                return reps;
            }

            free(currentFen);
        }


        return 0;
    }

    void commitMove(t_gameState move) {
        stateStack.push(*state);

        t_gameState *moveMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(moveMem, &move, sizeof(t_gameState));
        state = moveMem;

        turn = !turn;
        moveCounter++;

        // positionTracking();
        // TODO: Check for game ends?
        // TODO: Stuff with move time?
    }

    void revertMove() {
        // positionTrackingUndo();

        free(state);

        t_gameState *stateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(stateMem, &stateStack.top(), sizeof(t_gameState));  // Reset game state to last state
        state = stateMem;

        stateStack.pop();  // Remove the (now) current move from the stack

        turn = !turn;
        moveCounter--;

        isOver = false;
        whiteWon = false;
        blackWon = false;
    }

    t_board board() const {
        return state->board;
    }

    char *FEN() const {
        t_board board = state->board;
        return getFen(&board);
    }
} t_game;

t_gameOld *startGame(uint64_t gameTime);
void play(int maxRounds, uint64_t gameTime);

#endif //KINGOFTHEHILL_KI_GAME_H
