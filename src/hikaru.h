#ifndef KINGOFTHEHILL_KI_HIKARU_H
#define KINGOFTHEHILL_KI_HIKARU_H


#include <cstdint>
#include <ctime>

#include "util.h"
#include "move.h"
#include "game.h"
#include "pieceSquareTable.h"
#include "end.h"

#define KING_VALUE 20000
#define QUEEN_VALUE 9
#define ROOK_VALUE 5
#define BISHOP_VALUE 3
#define KNIGHT_VALUE 3
#define PAWN_VALUE 1



// calculates the time left finding a move
static uint64_t timeLeft(uint64_t timePerMove, time_t startMoveTime) {
    time_t currentTime = time(nullptr);
    return currentTime - timePerMove - startMoveTime;
}

#include <algorithm>
void printMoveStack(t_game *game, int depth) {
    std::stack<t_gameState> moveStack = game->stateStack;

    std::vector<t_gameState> debugVector = std::vector<t_gameState>();
    while (!moveStack.empty() && debugVector.size() < depth - 1) {
        t_gameState t = moveStack.top();
        debugVector.push_back(t);
        moveStack.pop();
    }

    // stack, read from top down, is reversed relative to its creation (from bot to top)
    for (int i = (int )debugVector.size()-1; i >= 0; i--) {
        t_gameState currentState = debugVector.at(i);

        printMove(currentState, '\t');
        game->stateStack.push(currentState);
    }

    printMove(*game->state, '\t');
}


inline float evaluate(t_game* game) {
    // Simple approach to evaluating positions by taking a look at the available material
    if (game->isOver) {
        if (game->whiteWon) {
            // White won -> Return max value minus move counter to prioritize faster wins
            return std::numeric_limits<float>::max() - (float )game->moveCounter / 2;
        } else if (game->blackWon) {
            // Black won -> Return min value plus move counter to prioritize faster wins
            return -std::numeric_limits<float>::max() + (float )game->moveCounter / 2;
        } else {
            // Draw -> Return neutral score, as neither side should get any scores out of it
            int sign = (game->turn * 2) - 1;
            return (float )sign / (float )(std::pow(game->moveCounter / 2, 2) + 1);
        }
    }

    float score = 0;

    score += (float )countFigure(game->board().whiteQueen) * QUEEN_VALUE;
    score += (float )countFigure(game->board().whiteRook) * ROOK_VALUE;
    score += (float )countFigure(game->board().whiteBishop) * BISHOP_VALUE;
    score += (float )countFigure(game->board().whiteKnight) * KNIGHT_VALUE;
    score += (float )countFigure(game->board().whitePawn) * PAWN_VALUE;

    score -= (float )countFigure(game->board().blackQueen) * QUEEN_VALUE;
    score -= (float )countFigure(game->board().blackRook) * ROOK_VALUE;
    score -= (float )countFigure(game->board().blackBishop) * BISHOP_VALUE;
    score -= (float )countFigure(game->board().blackKnight) * KNIGHT_VALUE;
    score -= (float )countFigure(game->board().blackPawn ) * PAWN_VALUE;


    t_board board = game->board();

    // ------------ //
    // White pieces //
    // ------------ //
    
    // King
    short wKingShift = findFirst(board.whiteKing);
    score += (float )pst_king_white[wKingShift];

    // Queens
    field wQueens = board.whiteQueen;
    while (wQueens != 0) {
        short shift = findFirst(wQueens);
        score += QUEEN_VALUE + pst_queen_white[shift];
        wQueens &= (wQueens - 1);
    }
    
    // Rooks
    field wRooks = board.whiteRook;
    while (wRooks != 0) {
        short shift = findFirst(wRooks);
        score += ROOK_VALUE + pst_rook_white[shift];
        wRooks &= (wRooks - 1);
    }
    
    // Bishops
    field wBishops = board.whiteBishop;
    while (wBishops != 0) {
        short shift = findFirst(wBishops);
        score += BISHOP_VALUE + pst_bishop_white[shift];
        wBishops &= (wBishops - 1);
    }
    
    // Knight
    field wKnights = board.whiteKnight;
    while (wKnights != 0) {
        short shift = findFirst(wKnights);
        score += KNIGHT_VALUE + pst_knight_white[shift];
        wKnights &= (wKnights - 1);
    }
    
    // Pawns
    field wPawns = board.whitePawn;
    while (wPawns != 0) {
        short shift = findFirst(wPawns);
        score += PAWN_VALUE + pst_pawn_white[shift];
        wPawns &= (wPawns - 1);
    }
    
    
    // ------------ //
    // Black pieces //
    // ------------ //

    // King
    short bKingShift = findFirst(board.blackKing);
    score -= (float )pst_king_black[bKingShift];

    // Queens
    field bQueens = board.blackQueen;
    while (bQueens != 0) {
        short shift = findFirst(bQueens);
        score -= QUEEN_VALUE + pst_queen_black[shift];
        bQueens &= (bQueens - 1);
    }

    // Rooks
    field bRooks = board.blackRook;
    while (bRooks != 0) {
        short shift = findFirst(bRooks);
        score -= ROOK_VALUE + pst_rook_black[shift];
        bRooks &= (bRooks - 1);
    }

    // Bishops
    field bBishops = board.blackBishop;
    while (bBishops != 0) {
        short shift = findFirst(bBishops);
        score -= BISHOP_VALUE + pst_bishop_black[shift];
        bBishops &= (bBishops - 1);
    }

    // Knight
    field bKnights = board.blackKnight;
    while (bKnights != 0) {
        short shift = findFirst(bKnights);
        score -= KNIGHT_VALUE + pst_knight_black[shift];
        bKnights &= (bKnights - 1);
    }

    // Pawns
    field bPawns = board.blackPawn;
    while (bPawns != 0) {
        short shift = findFirst(bPawns);
        score -= PAWN_VALUE + pst_pawn_black[shift];
        bPawns &= (bPawns - 1);
    }


//    printMoveStack(game, 2);
//    printf("=> %6.4f\n", score);

    return score;
}


template<bool color>
static inline t_gameState getMoveRandom(t_game *game) {
    std::vector<t_gameState> possibleMoves = generate_moves<color>(*game->state);

    if (possibleMoves.empty()) {
        return { game->board(), t_move() };
    }

    // Generate random number within bounds of possible moves list and return the corresponding move
    int randomMoveIndex = randn(0, (int )possibleMoves.size());
    return possibleMoves.at(randomMoveIndex);
}

template<bool color>
static inline float alphaBeta(int depth, float alpha, float beta, t_game *game, uint64_t timePerMove, time_t startMoveTime) {

    if (depth == 0 || game->isOver || timeLeft(timePerMove, startMoveTime) <= 0) {
        return evaluate(game);
    }

    float score, bestScore;

    if constexpr (color) {
        std::vector<t_gameState> moves = generate_moves<true>(*game->state);
        bestScore = std::numeric_limits<float>::max();

        if (moves.empty()) {
            winner_t endType = checkEndNoMoves(false, game->state);

            game->isOver = true;
            if (endType == winner_t::WHITE) {
                game->whiteWon = true;
            }
            if (endType == winner_t::BLACK) {
                game->blackWon = true;
            }

            return evaluate(game);
        }

        // Black's turn -> Minimize score
        for (auto currentMove : moves) {
            game->commitMove(currentMove);
            score = alphaBeta<false>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            game->revertMove();

            bestScore = min(bestScore, score);
            beta = min(beta, bestScore);

            if (beta <= alpha) {
                // BETA CUTOFF: "Opposing" team would always choose one of the already better moves
                break;
            }
        }

        return bestScore;
    } else {
        std::vector<t_gameState> moves = generate_moves<false>(*game->state);
        bestScore = -std::numeric_limits<float>::max();

        if (moves.empty()) {
            winner_t endType = checkEndNoMoves(true, game->state);

            game->isOver = true;
            if (endType == winner_t::WHITE) {
                game->whiteWon = true;
            }
            if (endType == winner_t::BLACK) {
                game->blackWon = true;
            }

            return evaluate(game);
        }

        // White's turn -> Maximize score
        for (auto currentMove : moves) {
            game->commitMove(currentMove);
            score = alphaBeta<true>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            game->revertMove();

            bestScore = max(bestScore, score);
            alpha = max(alpha, bestScore);

            if (beta <= alpha) {
                // ALPHA CUTOFF: "Opposing" team would always choose one of the already better moves
                break;
            }
        }
    }

    return bestScore;
}

//template<float color>
//std::pair<t_gameState, float> alphaBetaHead(t_game *game, int max_depth, uint64_t timePerMove) {
//    int depth = max_depth;  // NOTE: Always use one less than actually wanted
//
//    float alpha = -std::numeric_limits<float>::max();
//    float beta = std::numeric_limits<float>::max();
//    time_t startMoveTime = time(nullptr);
//
//    float score, bestScore;
//    t_gameState zeroMove = t_gameState(game->board(), t_move());
//    t_gameState *bestMove = &zeroMove;
//
//    if (color) {
//        std::vector<t_gameState> moves = generate_moves<true>(*game->state);
//        bestScore = -std::numeric_limits<float>::max();
//
//        // White's turn -> Maximize score
//        for (auto currentMove : moves) {
//            game->commitMove(currentMove);
//            score = alphaBeta<false>(depth - 1, alpha, beta, game, timePerMove, startMoveTime);
//            game->revertMove();
//
//            if (score > bestScore) {
//                bestScore = score;
//                bestMove = &currentMove;
//            }
//
//            alpha = max(alpha, bestScore);
//        }
//    } else {
//        std::vector<t_gameState> moves = generate_moves<false>(*game->state);
//        bestScore = std::numeric_limits<float>::max();
//
//        // Black's turn -> Minimize score
//        for (auto currentMove : moves) {
//            game->commitMove(currentMove);
//            score = alphaBeta<true>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
//            game->revertMove();
//
//            if (score < bestScore) {
//                bestScore = score;
//                bestMove = &currentMove;
//            }
//
//            beta = min(beta, bestScore);
//        }
//    }
//
//    return {*bestMove, bestScore};
//}


template<bool color>
static inline std::pair<t_gameState, float> alphaBetaHead(t_game *game, int max_depth, uint64_t timePerMove);


template<>
inline std::pair<t_gameState, float> alphaBetaHead<true>(t_game *game, int max_depth, uint64_t timePerMove) {
    int depth = max_depth;  // NOTE: Always use one less than actually wanted

    float alpha = -std::numeric_limits<float>::max();
    float beta = std::numeric_limits<float>::max();
    time_t startMoveTime = time(nullptr);

    float score, bestScore;
    t_gameState zeroMove = t_gameState(game->board(), t_move());
    t_gameState *bestMove = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
    memcpy(bestMove, &zeroMove, sizeof(t_gameState));

    std::vector<t_gameState> moves = generate_moves<true>(*game->state);
    bestScore = std::numeric_limits<float>::max();

    if (moves.empty()) {
        winner_t endType = checkEndNoMoves(false, game->state);

        game->isOver = true;
        if (endType == winner_t::WHITE) {
            game->whiteWon = true;
        }
        if (endType == winner_t::BLACK) {
            game->blackWon = true;
        }

        return {zeroMove, evaluate(game)};
    }

    // Black's turn -> Minimize score
    for (auto currentMove : moves) {
        game->commitMove(currentMove);
        score = alphaBeta<false>(depth - 1, alpha, beta, game, timePerMove, startMoveTime);
        game->revertMove();

        if (score <= bestScore) {
            bestScore = score;
            memcpy(bestMove, &currentMove, sizeof(t_gameState));
        }

        beta = min(beta, bestScore);
    }

    t_gameState returnMove = t_gameState(*bestMove);
    free(bestMove);

    return {returnMove, bestScore};
}


template<>
inline std::pair<t_gameState, float> alphaBetaHead<false>(t_game *game, int max_depth, uint64_t timePerMove) {
    int depth = max_depth;  // NOTE: Always use one less than actually wanted

    float alpha = -std::numeric_limits<float>::max();
    float beta = std::numeric_limits<float>::max();
    time_t startMoveTime = time(nullptr);

    float score, bestScore;
    t_gameState zeroMove = t_gameState(game->board(), t_move());
    t_gameState *bestMove = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
    memcpy(bestMove, &zeroMove, sizeof(t_gameState));

    std::vector<t_gameState> moves = generate_moves<false>(*game->state);
    bestScore = -std::numeric_limits<float>::max();

    if (moves.empty()) {
        winner_t endType = checkEndNoMoves(true, game->state);

        game->isOver = true;
        if (endType == winner_t::WHITE) {
            game->whiteWon = true;
        }
        if (endType == winner_t::BLACK) {
            game->blackWon = true;
        }

        return {zeroMove, evaluate(game)};
    }

    // White's turn -> Maximize score
    for (auto currentMove : moves) {
        game->commitMove(currentMove);
        score = alphaBeta<true>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
        game->revertMove();

        if (score >= bestScore) {
            bestScore = score;
            memcpy(bestMove, &currentMove, sizeof(t_gameState));
        }

        alpha = max(alpha, bestScore);
    }

    t_gameState returnMove = t_gameState(*bestMove);
    free(bestMove);

    return {returnMove, bestScore};
}


template<bool color>
inline std::pair<gameState, float> getMove(t_game *game, uint64_t timePerMove) {
    return alphaBetaHead<color>(game, 3, timePerMove);
}


template<>
inline std::pair<gameState, float> getMove<true>(t_game *game, uint64_t timePerMove) {
    return alphaBetaHead<true>(game, 3, timePerMove);
}


template<>
inline std::pair<gameState, float> getMove<false>(t_game *game, uint64_t timePerMove) {
    return alphaBetaHead<false>(game, 3, timePerMove);
}

#endif //KINGOFTHEHILL_KI_HIKARU_H
