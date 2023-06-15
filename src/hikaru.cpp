#include "hikaru.h"
#include "move.h"
#include "game.h"
#include "util.h"
#include <cstdlib>
#include <ctime>
#include "hikaru.h"
#include "end.h"
#include "pieceSquareTable.h"
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
uint64_t timeLeft(uint64_t timePerMove, time_t startMoveTime) {
    time_t currentTime = time(NULL);
    return currentTime - timePerMove - startMoveTime;
}


float evaluate(const t_game* game, t_move* lastMove) {
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

    // count pieces
    float score = 0;
    uint64_t whiteColorFilter = game->board->white;
    uint64_t blackColorFilter = game->board->black;

    // evaluate move according to piece square table

    // WHITE

    // King
    std::vector<Position> kingPositions = board_value_positions(game->board->king & whiteColorFilter);
    Position kingPosition = kingPositions.at(0);
    score += KING_VALUE + pst_king_white[shift_from_position(kingPosition)];

    // Queen
    std::vector<Position> queenPositions = board_value_positions(game->board->queen & whiteColorFilter);
    for (auto queenPosition : queenPositions) {
        score += QUEEN_VALUE + pst_queen_white[shift_from_position(queenPosition)];
    }

    // Rook
    std::vector<Position> rookPositions = board_value_positions(game->board->rook & whiteColorFilter);
    for (auto rookPosition : rookPositions) {
        score += ROOK_VALUE + pst_rook_white[shift_from_position(rookPosition)];
    }

    // Bishop
    std::vector<Position> bishopPositions = board_value_positions(game->board->bishop & whiteColorFilter);
    for (auto bishopPosition : bishopPositions) {
        score += BISHOP_VALUE + pst_bishop_white[shift_from_position(bishopPosition)];
    }

    // Knight
    std::vector<Position> knightPositions = board_value_positions(game->board->knight & whiteColorFilter);
    for (auto knightPosition : knightPositions) {
        score += KNIGHT_VALUE + pst_knight_white[shift_from_position(knightPosition)];
    }

    // Pawn
    std::vector<Position> pawnPositions = board_value_positions(game->board->pawn & whiteColorFilter);
    for (auto pawnPosition : pawnPositions) {
        score += PAWN_VALUE + pst_pawn_white[shift_from_position(pawnPosition)];
    }


    // BLACK

    // King
    std::vector<Position> kingPositionsBlack = board_value_positions(game->board->king & blackColorFilter);
    Position kingPositionBlack = kingPositionsBlack.at(0);
    score -= KING_VALUE - pst_king_black[shift_from_position(kingPositionBlack)];

    // Queen
    std::vector<Position> queenPositionsBlack = board_value_positions(game->board->queen & blackColorFilter);
    for (auto queenPositionBlack : queenPositionsBlack) {
        score -= QUEEN_VALUE - pst_queen_black[shift_from_position(queenPositionBlack)];
    }

    // Rook
    std::vector<Position> rookPositionsBlack = board_value_positions(game->board->rook & blackColorFilter);
    for (auto rookPositionBlack : rookPositionsBlack) {
        score -= ROOK_VALUE - pst_rook_black[shift_from_position(rookPositionBlack)];
    }

    // Bishop
    std::vector<Position> bishopPositionsBlack = board_value_positions(game->board->bishop & blackColorFilter);
    for (auto bishopPositionBlack : bishopPositionsBlack) {
        score -= BISHOP_VALUE - pst_bishop_black[shift_from_position(bishopPositionBlack)];
    }

    // Knight
    std::vector<Position> knightPositionsBlack = board_value_positions(game->board->knight & blackColorFilter);
    for (auto knightPositionBlack : knightPositionsBlack) {
        score -= KNIGHT_VALUE - pst_knight_black[shift_from_position(knightPositionBlack)];
    }

    // Pawn
    std::vector<Position> pawnPositionsBlack = board_value_positions(game->board->pawn & blackColorFilter);
    for (auto pawnPositionBlack : pawnPositionsBlack) {
        score -= PAWN_VALUE - pst_pawn_black[shift_from_position(pawnPositionBlack)];
    }

    return score;
}

float alphaBeta(int depth, float alpha, float beta, t_game *game, t_move* lastMove,  uint64_t timePerMove, time_t startMoveTime) {

    if (depth == 0 || game->isOver) { // || timeLeft(timePerMove, startMoveTime) <= 0
        return evaluate(game, lastMove);
    }

    float score, bestScore;
    std::vector<t_move> moves = generate_moves(game, game->turn);

    for (int _ = 0; _ < depth; ++_) {
        // printf("\t");
    }
    // printf("Found %zu moves\n", moves.size());

    // maximising
    if (!game->turn) {
        bestScore = -std::numeric_limits<float>::max();

        // White's turn -> Maximize score
        for (auto currentMove : moves) {
            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game, &currentMove, timePerMove, startMoveTime);
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
            score = alphaBeta(depth-1, alpha, beta, game, &currentMove, timePerMove, startMoveTime);
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

std::pair<t_move, float> alphaBetaHead(t_game* game, int max_depth, uint64_t timePerMove) {
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
            score = alphaBeta(depth-1, alpha, beta, game, nullptr, timePerMove, startMoveTime);
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
            score = alphaBeta(depth-1, alpha, beta, game, nullptr, timePerMove, startMoveTime);
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


t_move getMove(t_game *game, bool color, uint64_t timePerMove) {
    // t_move bestMove = getMoveRandom(game, color);
    std::pair<t_move, float> abReturn = alphaBetaHead(game, 4, timePerMove);

    t_move bestMove = abReturn.first;
    float bestScore = abReturn.second;
    printf("SELECTED ");
    printMove(bestMove);
    printf(" with score %.4f\n", bestScore);

    return bestMove;
}

