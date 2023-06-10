#include "hikaru.h"
#include "move.h"
#include "game.h"
#include "util.h"
#include <cstdlib>
#include <ctime>
#include "hikaru.h"
#include "end.h"
#include <limits>
#include <time.h>



t_move getMoveRandom(t_game *game, bool color) {
    std::vector<t_move> possibleMoves = generate_moves(game, color);

    if (possibleMoves.empty()) {
        t_move failureMove = { 0, 0, 0, 0 };
        return failureMove;
    }

//    // printf("Found %zu moves!\n", possibleMoves.length());
//    for (int i = 0; i < possibleMoves.length(); ++i) {
//        // printMove(possibleMoves.get(i));
//    }

    // Initialize rng
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned ) ts.tv_nsec);

    // Generate random number within bounds of possible moves list and return the corresponding move
    int randomMoveIndex = (int )(rand() % possibleMoves.size());
    return possibleMoves.at(randomMoveIndex);
}

int randn(int start, int stop) {
    // Initialize rng
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned ) ts.tv_nsec);

    // Generate and return random number within specified bounds
    return (int )((rand() % (stop - start)) + start);
}


// calculates the time left finding a move
field timeLeft(field timePerMove, time_t startMoveTime) {
    time_t currentTime = time(NULL);
    return currentTime - timePerMove - startMoveTime;
}


float evaluate(const t_game* game) {
    // Simple approach to evaluating positions by taking a look at the available material
    if (game->isOver) {
        if (game->whiteWon) {
            // White won -> Return max value minus move counter to prioritize faster wins
            return std::numeric_limits<float>::max() - (float )game->whiteMoveCounter;
        } else if (game->blackWon) {
            // Black won -> Return min value plus move counter to prioritize faster wins
            return -std::numeric_limits<float>::max() + (float )game->blackMoveCounter;
        } else {
            // Draw -> Return neutral score, as neither side should get any scores out of it
            int sign = (game->turn * 2) - 1;
            return (float )sign / (float )(game->blackMoveCounter * game->blackMoveCounter + 1);
        }
    }

    float score = 0;

    score += (float )countFigure(game->board->whiteQueen) * QUEEN_VALUE;
    score += (float )countFigure(game->board->whiteRook) * ROOK_VALUE;
    score += (float )countFigure(game->board->whiteBishop) * BISHOP_VALUE;
    score += (float )countFigure(game->board->whiteKnight) * KNIGHT_VALUE;
    score += (float )countFigure(game->board->whitePawn) * PAWN_VALUE;

    score -= (float )countFigure(game->board->blackQueen) * QUEEN_VALUE;
    score -= (float )countFigure(game->board->blackRook) * ROOK_VALUE;
    score -= (float )countFigure(game->board->blackBishop) * BISHOP_VALUE;
    score -= (float )countFigure(game->board->blackKnight) * KNIGHT_VALUE;
    score -= (float )countFigure(game->board->blackPawn ) * PAWN_VALUE;

    return score;
}

float alphaBeta(int depth, float alpha, float beta, t_game *game,  field timePerMove, time_t startMoveTime) {

    if (depth == 0 || game->isOver || timeLeft(timePerMove, startMoveTime) <= 0) {
        return evaluate(game);
    }

    float score, bestScore;
    std::vector<t_move> moves = generate_moves(game, game->turn);

    for (int _ = 0; _ < depth; ++_) {
        // printf("\t");
    }
    // printf("Found %zu moves\n", moves.size());

    if (!game->turn) {
        bestScore = -std::numeric_limits<float>::max();

        // White's turn -> Maximize score
        for (auto currentMove : moves) {
            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                // printf("\t");
            }
            // printMove(currentMove);
            // printf(" returned value %.4f (current/new max is %.4f/", score, bestScore);

            bestScore = max(bestScore, score);
            alpha = max(alpha, bestScore);

            // printf("%.4f)\n", bestScore);

            if (beta <= alpha) {
                // "Opposing" team would always choose one of the already better moves
                for (int _ = 0; _ < depth; ++_) {
                    // printf("\t");
                }
                // printf("Beta Cutoff (%.4f <= %.4f)\n", beta, alpha);
                break;
            }
        }

        return bestScore;
    } else {
        bestScore = std::numeric_limits<float>::max();

        // Black's turn -> Minimize score
        for (auto currentMove : moves) {
            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                // printf("\t");
            }
            // printMove(currentMove);
            // printf(" returned value %.4f (current/new min is %.4f/", score, bestScore);

            bestScore = min(bestScore, score);
            beta = min(beta, bestScore);

            // printf("%.4f)\n", bestScore);

            if (beta <= alpha) {
                // "Opposing" team would always choose one of the already better moves
                for (int _ = 0; _ < depth; ++_) {
                    // printf("\t");
                }
                // printf("Alpha Cutoff (%.4f <= %.4f)\n", alpha, beta);
                break;
            }
        }
    }

    return bestScore;
}

std::pair<t_move, float> alphaBetaHead(t_game* game, int max_depth, field timePerMove) {
    int depth = max_depth;  // NOTE: Always use one less than actually wanted

    float alpha = -std::numeric_limits<float>::max();
    float beta = std::numeric_limits<float>::max();
    time_t startMoveTime = time(NULL);

    float score, bestScore;
    t_move currentMove, bestMove;
    std::vector<t_move> moves = generate_moves(game, game->turn);

//    if (!moves.empty()) {
//        // Default to first move
//        bestMove = moves.get(0);
//    }

    for (int _ = 0; _ < depth; ++_) {
        // printf("\t");
    }
    // printf("Found %zu moves\n", moves.size());

    // Set default best move as abort move recognized in play() function
    bestMove.origin = 0;
    bestMove.target = 0;

    if (!game->turn) {
        bestScore = -std::numeric_limits<float>::max();

        // White's turn -> Maximize score
        for (auto move : moves) {
            currentMove = move;

            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                // printf("\t");
            }
            // printMove(currentMove);
            // printf(" returned value %.4f (current/new max is %.4f/", score, bestScore);

            if (score > bestScore) {
                bestScore = score;
                bestMove = currentMove;
            }

            alpha = max(alpha, bestScore);

            // printf("%.4f)\n", bestScore);
        }
        return {bestMove, bestScore};
    } else {
        bestScore = std::numeric_limits<float>::max();

        // Black's turn -> Minimize score
        for (auto move : moves) {
            currentMove = move;

            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game, timePerMove, startMoveTime);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                // printf("\t");
            }
            // printMove(currentMove);
            // printf(" returned value %.4f (current/new min is %.4f/", score, bestScore);

            if (score < bestScore) {
                bestScore = score;
                bestMove = currentMove;
            }

            beta = min(beta, bestScore);

            // printf("%.4f)\n", bestScore);
        }
    }

    // printf("Returning move ");
    // printMove(bestMove);
    // printf("\n");

    return {bestMove, bestScore};
}


t_move getMove(t_game *game, bool color, field timePerMove) {
    // t_move bestMove = getMoveRandom(game, color);
    std::pair<t_move, float> abReturn = alphaBetaHead(game, 4, timePerMove);

    t_move bestMove = abReturn.first;
    float bestScore = abReturn.second;
    printf("SELECTED ");
    printMove(bestMove);
    printf(" with score %.4f\n", bestScore);

    return bestMove;
}

