#include "hikaru.h"
#include "move.h"
#include "game.h"
#include "util.h"
#include <cstdlib>
#include <ctime>
#include "hikaru.h"
#include "end.h"



t_move getMoveRandom(t_game *game, bool color) {
    List<t_move> possibleMoves = generate_moves(game, color);

    if (possibleMoves.empty()) {
        t_move failureMove = { 0, 0, 0, 0 };
        return failureMove;
    }

//    printf("Found %zu moves!\n", possibleMoves.length());
//    for (int i = 0; i < possibleMoves.length(); ++i) {
//        printMove(possibleMoves.get(i));
//    }

    // Initialize rng
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned ) ts.tv_nsec);

    // Generate random number within bounds of possible moves list and return the corresponding move
    int randomMoveIndex = (int )(rand() % possibleMoves.length());
    return possibleMoves.get(randomMoveIndex);
}


uint32_t evaluate(const t_game* game) {
    // Simple approach to evaluating positions by taking a look at the available material
    int score = 0;
    uint64_t whiteColorFilter = game->board->white;
    uint64_t blackColorFilter = game->board->black;

    score += countFigure(game->board->queen & whiteColorFilter) * QUEEN_VALUE;
    score += countFigure(game->board->rook & whiteColorFilter) * ROOK_VALUE;
    score += countFigure(game->board->bishop & whiteColorFilter) * BISHOP_VALUE;
    score += countFigure(game->board->knight & whiteColorFilter) * KNIGHT_VALUE;
    score += countFigure(game->board->pawn & whiteColorFilter) * PAWN_VALUE;

    score -= countFigure(game->board->queen & blackColorFilter) * QUEEN_VALUE;
    score -= countFigure(game->board->rook & blackColorFilter) * ROOK_VALUE;
    score -= countFigure(game->board->bishop & blackColorFilter) * BISHOP_VALUE;
    score -= countFigure(game->board->knight & blackColorFilter) * KNIGHT_VALUE;
    score -= countFigure(game->board->pawn & blackColorFilter) * PAWN_VALUE;

    return score;
}

int alphaBeta(int depth, int alpha, int beta, t_game *game) {
    if (depth == 0 || game->isOver) {
        return evaluate(game);
    }

    int score, bestScore;
    t_move currentMove;
    List<t_move> moves = generate_moves(game, game->turn);

    for (int _ = 0; _ < depth; ++_) {
        printf("\t");
    }
    printf("Found %zu moves\n", moves.length());

    if (!game->turn) {
        bestScore = INT32_MIN;

        // White's turn -> Maximize score
        for (int moveIndex = 0; moveIndex < moves.length(); ++moveIndex) {
            currentMove = moves.pop();

            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                printf("\t");
            }
            printMove(currentMove);
            printf(" returned value %d (current/new max is %d/", score, bestScore);

            bestScore = max(bestScore, score);
            alpha = max(alpha, score);

            printf("%d)\n", bestScore);

            if (beta <= alpha) {
                // "Opposing" team would always choose one of the already better moves
                break;
            }
        }
    } else {
        bestScore = INT32_MAX;

        // Black's turn -> Minimize score
        for (int moveIndex = 0; moveIndex < moves.length(); ++moveIndex) {
            currentMove = moves.pop();

            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                printf("\t");
            }
            printMove(currentMove);
            printf(" returned value %d (current/new max is %d/", score, bestScore);

            bestScore = min(bestScore, score);
            beta = min(beta, score);

            printf("%d)\n", bestScore);

            if (beta <= alpha) {
                // "Opposing" team would always choose one of the already better moves
                break;
            }
        }
    }

    return bestScore;
}

t_move alphaBetaHead(t_game* game, int max_depth) {
    int depth = max_depth;  // NOTE: Always use one less than actually wanted

    int alpha = INT32_MIN;
    int beta = INT32_MAX;

    int score, bestScore;
    t_move currentMove, bestMove;
    List<t_move> moves = generate_moves(game, game->turn);

    for (int _ = 0; _ < depth; ++_) {
        printf("\t");
    }
    printf("Found %zu moves\n", moves.length());

    // Set default best move as abort move recognized in play() function
    bestMove.origin = 0;
    bestMove.target = 0;

    if (!game->turn) {
        bestScore = INT32_MIN;

        // White's turn -> Maximize score
        for (int moveIndex = 0; moveIndex < moves.length(); ++moveIndex) {
            currentMove = moves.pop();

            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                printf("\t");
            }
            printMove(currentMove);
            printf(" returned value %d (current/new max is %d/", score, bestScore);

            if (score > bestScore) {
                bestScore = score;
                bestMove = currentMove;
            }

            printf("%d)\n", bestScore);
        }
    } else {
        bestScore = INT32_MAX;

        // Black's turn -> Minimize score
        for (int moveIndex = 0; moveIndex < moves.size(); ++moveIndex) {
            currentMove = moves.pop();

            commitMove(game, &currentMove);
            score = alphaBeta(depth-1, alpha, beta, game);
            revertMove(game, &currentMove);

            for (int _ = 0; _ < depth; ++_) {
                printf("\t");
            }
            printMove(currentMove);
            printf(" returned value %d (current/new min is %d/", score, bestScore);

            if (score < bestScore) {
                bestScore = score;
                bestMove = currentMove;
            }

            printf("%d)\n", bestScore);
        }
    }

    return bestMove;
}


t_move getMove(t_game *game, bool color) {
    // t_move bestMove = getMoveRandom(game, color);
    t_move bestMove = alphaBetaHead(game, 2);
    printf("SELECTED ");
    printMove(bestMove);
    printf("\n");

    return bestMove;
}

