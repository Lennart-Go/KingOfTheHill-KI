#include "board.h"
#include "move.h"
#include "game.h"

#ifndef KINGOFTHEHILL_KI_HIKARU_H
#define KINGOFTHEHILL_KI_HIKARU_H

#define KING_VALUE 20000
#define QUEEN_VALUE 900
#define ROOK_VALUE 500
#define BISHOP_VALUE 330
#define KNIGHT_VALUE 320
#define PAWN_VALUE 100


// calculates the time left finding a move
static uint64_t timeLeft(uint64_t timePerMove, time_t startMoveTime) {
    time_t currentTime = time(nullptr);
    return currentTime - timePerMove - startMoveTime;
}

inline float evaluate(const t_game* game) {
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

    if (color) {
        std::vector<t_gameState> moves = generate_moves<true>(*game->state);
        bestScore = -std::numeric_limits<float>::max();

        // White's turn -> Maximize score
        for (auto currentMove : moves) {
            game->commitMove(currentMove);
            score = alphaBeta<false>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            game->revertMove();

            bestScore = max(bestScore, score);
            alpha = max(alpha, bestScore);

            if (beta <= alpha) {
                // "Opposing" team would always choose one of the already better moves
                break;
            }
        }

        return bestScore;
    } else {
        std::vector<t_gameState> moves = generate_moves<false>(*game->state);
        bestScore = std::numeric_limits<float>::max();

        // Black's turn -> Minimize score
        for (auto currentMove : moves) {
            game->commitMove(currentMove);
            score = alphaBeta<true>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            game->revertMove();

            bestScore = min(bestScore, score);
            beta = min(beta, bestScore);

            if (beta <= alpha) {
                // "Opposing" team would always choose one of the already better moves
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
inline std::pair<t_gameState, float> alphaBetaHead(t_game *game, int max_depth, uint64_t timePerMove);


template<>
inline std::pair<t_gameState, float> alphaBetaHead<true>(t_game *game, int max_depth, uint64_t timePerMove) {
    int depth = max_depth;  // NOTE: Always use one less than actually wanted

    float alpha = -std::numeric_limits<float>::max();
    float beta = std::numeric_limits<float>::max();
    time_t startMoveTime = time(nullptr);

    float score, bestScore;
    t_gameState zeroMove = t_gameState(game->board(), t_move());
    t_gameState *bestMove = &zeroMove;

    std::vector<t_gameState> moves = generate_moves<true>(*game->state);
    bestScore = -std::numeric_limits<float>::max();

    // White's turn -> Maximize score
    for (auto currentMove : moves) {
        game->commitMove(currentMove);
        score = alphaBeta<false>(depth - 1, alpha, beta, game, timePerMove, startMoveTime);
        game->revertMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove = &currentMove;
        }

        alpha = max(alpha, bestScore);
    }

    return {*bestMove, bestScore};
}


template<>
inline std::pair<t_gameState, float> alphaBetaHead<false>(t_game *game, int max_depth, uint64_t timePerMove) {
    int depth = max_depth;  // NOTE: Always use one less than actually wanted

    float alpha = -std::numeric_limits<float>::max();
    float beta = std::numeric_limits<float>::max();
    time_t startMoveTime = time(nullptr);

    float score, bestScore;
    t_gameState zeroMove = t_gameState(game->board(), t_move());
    t_gameState *bestMove = &zeroMove;

    std::vector<t_gameState> moves = generate_moves<false>(*game->state);
    bestScore = std::numeric_limits<float>::max();

    // Black's turn -> Minimize score
    for (auto currentMove : moves) {
        game->commitMove(currentMove);
        score = alphaBeta<true>(depth-1, alpha, beta, game, timePerMove, startMoveTime);
        game->revertMove();

        if (score < bestScore) {
            bestScore = score;
            bestMove = &currentMove;
        }

        beta = min(beta, bestScore);
    }

    return {*bestMove, bestScore};
}


template<bool color>
inline std::pair<gameState, float> getMove(t_game *game, uint64_t timePerMove) {
    return alphaBetaHead<color>(game, 7, timePerMove);
}


template<>
inline std::pair<gameState, float> getMove<true>(t_game *game, uint64_t timePerMove) {
    return alphaBetaHead<true>(game, 7, timePerMove);
}


template<>
inline std::pair<gameState, float> getMove<false>(t_game *game, uint64_t timePerMove) {
    return alphaBetaHead<false>(game, 7, timePerMove);
}

#endif //KINGOFTHEHILL_KI_HIKARU_H
