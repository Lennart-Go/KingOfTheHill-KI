#ifndef KINGOFTHEHILL_KI_GAME_H
#define KINGOFTHEHILL_KI_GAME_H


#include <map>
#include <stack>
#include <string>
#include <cstring>
#include <chrono>

#include "board.h"
#include "move.h"
#include "transpositionTable.h"
#include "hash.h"
#include "end.h"


typedef struct game {
    t_gameState *state;
    std::stack<t_gameState> stateStack;

    uint64_t *random;
    std::map<uint64_t, int> *positionHistory = nullptr;

    bool turn;

    double gameTime;
    short averageMoveCount = 20;

    double whiteMoveTime;
    std::chrono::steady_clock::time_point whiteLastMoveTime;
    short whiteMovesRemaining = 40;

    double blackMoveTime;
    std::chrono::steady_clock::time_point blackLastMoveTime;
    short blackMovesRemaining = 40;


    bool isOver;
    bool whiteWon;
    bool blackWon;

    short moveCounter;

    TranspositionTable tableWhite;
    TranspositionTable tableBlack;

    game(game const &other) {
        state = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(state, other.state, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>(other.stateStack);
        random = other.random;
        turn = other.turn;

        gameTime = other.gameTime;
        whiteMoveTime = other.whiteMoveTime;
        blackMoveTime = other.blackMoveTime;
        whiteLastMoveTime = other.whiteLastMoveTime;
        blackLastMoveTime = other.blackLastMoveTime;

        isOver = other.isOver;
        whiteWon = other.whiteWon;
        blackWon = other.blackWon;

        moveCounter = other.moveCounter;

        tableWhite = TranspositionTable(other.tableWhite);
        tableBlack = TranspositionTable(other.tableBlack);
    }

    explicit game(uint64_t time) {
        // Default game constructor
        t_board startBoard = setFen((char *)"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

        t_gameState startState = t_gameState(startBoard);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        random = init_hash();
        turn = false;

        gameTime = (double )time;
        whiteMoveTime = (double )time;
        blackMoveTime = (double )time;
        whiteLastMoveTime = std::chrono::steady_clock::now();
        blackLastMoveTime = std::chrono::steady_clock::now();

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;

        tableWhite = TranspositionTable();
        tableBlack = TranspositionTable();
    }
    game(char *startFen, bool color, uint64_t time) {
        // Game constructor from FEN and side to move
        t_board startBoard = setFen(startFen);

        t_gameState startState = t_gameState(startBoard);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        random = init_hash();
        turn = color;

        gameTime = (double )time;
        whiteMoveTime = (double )time;
        blackMoveTime = (double )time;
        whiteLastMoveTime = std::chrono::steady_clock::now();
        blackLastMoveTime = std::chrono::steady_clock::now();

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;

        tableWhite = TranspositionTable();
        tableBlack = TranspositionTable();
    }
    game(char *startFen, bool color, uint8_t castleCode, uint64_t time) {
        // Game constructor from FEN, side to move and castling code
        t_board startBoard = setFen(startFen);

        bool wCastleShort = (castleCode & 0b0001) != 0;
        bool wCastleLong = (castleCode & 0b0010) != 0;
        bool bCastleShort = (castleCode & 0b0100) != 0;
        bool bCastleLong = (castleCode & 0b1000) != 0;

        t_gameState startState = t_gameState(startBoard,
                                             wCastleShort, wCastleLong,
                                             bCastleShort, bCastleLong,
                                             0);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        random = init_hash();
        turn = color;

        gameTime = (double )time;
        whiteMoveTime = (double )time;
        blackMoveTime = (double )time;
        whiteLastMoveTime = std::chrono::steady_clock::now();
        blackLastMoveTime = std::chrono::steady_clock::now();

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;

        tableWhite = TranspositionTable();
        tableBlack = TranspositionTable();
    }
    game(char *startFen, bool color, uint8_t castleCode, uint8_t ep, uint64_t time) {
        // Game constructor from FEN, side to move and castling code
        t_board startBoard = setFen(startFen);

        bool wCastleShort = (castleCode & 0b0001) != 0;
        bool wCastleLong = (castleCode & 0b0010) != 0;
        bool bCastleShort = (castleCode & 0b0100) != 0;
        bool bCastleLong = (castleCode & 0b1000) != 0;

        t_gameState startState = t_gameState(startBoard,
                                             wCastleShort, wCastleLong,
                                             bCastleShort, bCastleLong,
                                             ep);
        t_gameState *startStateMem = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
        memcpy(startStateMem, &startState, sizeof(t_gameState));

        stateStack = std::stack<t_gameState>();
        state = startStateMem;

        random = init_hash();
        turn = color;

        gameTime = (double )time;
        whiteMoveTime = (double )time;
        blackMoveTime = (double )time;
        whiteLastMoveTime = std::chrono::steady_clock::now();
        blackLastMoveTime = std::chrono::steady_clock::now();

        isOver = false;
        whiteWon = false;
        blackWon = false;

        moveCounter = 0;

        tableWhite = TranspositionTable();
        tableBlack = TranspositionTable();
    }

    void updateAverageMoves(short moveCount) {
        averageMoveCount = (short )((3 * averageMoveCount + moveCount) / 4);
    }

    void positionTracking() {
        /* Function to check if same position occurred for the third time
        * Arguments:
        *  t_gameOld *gameOld: Pointer to the gameOld representing the state of the gameOld
        */
        uint64_t boardHash = hash(random, state);
        std::map<uint64_t, int> &map = *positionHistory;

        if (positionHistory == nullptr) {
            std::map<uint64_t, int> *mapInit = new std::map<uint64_t, int>();
            positionHistory = mapInit;

            positionHistory->insert(std::make_pair(boardHash, 1));
        } else {
            if (map.find(boardHash) != map.end()) {
                int n = map[boardHash];
                map[boardHash] = n + 1;
            } else {
                map.insert(std::make_pair(boardHash, 1));
            }
        }
    }

    void positionTrackingUndo() const {
        uint64_t boardHash = hash(random, state);
        std::map<uint64_t, int> &map = *positionHistory;

        if (positionHistory != nullptr) {
            if (map.find(boardHash) != map.end()) {
                int n = map[boardHash];

                if (n > 0) {
                    map[boardHash] = n - 1;
                }
            }
        }
    }

    int positionRepetitions() const {
        uint64_t boardHash = hash(random, state);
        std::map<uint64_t, int> &map = *positionHistory;

        if (positionHistory != nullptr) {
            if (map.find(boardHash) != map.end()) {
                return map[boardHash];
            }
        }

        return 0;
    }

    void commitMove(t_gameState move) {
        stateStack.push(*state);

        memcpy(state, &move, sizeof(t_gameState));

        turn = !turn;
        moveCounter++;

        positionTracking();
        winner_t endType = checkEndLimited(!turn, state);
        if (endType == winner_t::WHITE) {
            isOver = true;
            whiteWon = true;
        } else if (endType == winner_t::BLACK) {
            isOver = true;
            blackWon = true;
        } else if (endType == winner_t::DRAW) {
            isOver = true;
        }
        if (positionRepetitions() > 2) {
            isOver = true;
        }

        // TODO: Stuff with move time?
    }

    void commitMoveTimed(t_gameState move) {
        std::chrono::time_point currentTime = std::chrono::steady_clock::now();
        if (turn) {
            // Black is moving
            std::chrono::nanoseconds moveTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - whiteLastMoveTime);
            blackMoveTime -= (double )moveTime.count() / 1e9;

            blackLastMoveTime = currentTime;
            blackMovesRemaining--;

            if (blackMoveTime < 0) {
                isOver = true;
                whiteWon = true;
            }

            printf("Move time remaining for black %.4fs/%d\n", blackMoveTime, blackMovesRemaining);
        } else {
            // White is moving
            std::chrono::nanoseconds moveTime = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - blackLastMoveTime);
            whiteMoveTime -= (double )moveTime.count() / 1e9;

            whiteLastMoveTime = currentTime;
            whiteMovesRemaining--;

            if (whiteMoveTime < 0) {
                isOver = true;
                blackWon = true;
            }

            printf("Move time remaining for white %.4fs/%d\n", whiteMoveTime, whiteMovesRemaining);
        }

        if (moveCounter % 80 == 79) {
            blackMoveTime = gameTime;
            whiteMoveTime = gameTime;

            blackMovesRemaining = 40;
            whiteMovesRemaining = 40;
        }

        commitMove(move);
    }

    void revertMove() {
        positionTrackingUndo();

        memcpy(state, &stateStack.top(), sizeof(t_gameState));  // Reset game state to last state

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

void play(int maxRounds, uint64_t gameTime);
void printMoveStack(t_game game);

#endif //KINGOFTHEHILL_KI_GAME_H
