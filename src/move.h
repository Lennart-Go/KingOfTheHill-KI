#include "util.h"
#include "board.h"
#include "game.h"

#ifndef KINGOFTHEHILL_KI_MOVE_H
#define KINGOFTHEHILL_KI_MOVE_H

/*
 * Move representation
 * Origin Position (1x6 bit)
 * Target Position (1x6 bit)
 * Taken Figure (3 bit)
 * Turn color (1 bit)
 * => Total 16 bit / 2 bytes
 */

typedef struct move {
    unsigned origin:6;
    unsigned target:6;
    unsigned taken_figure:3;  // 0 -> Non taken, 1 -> Queen, 2 -> Rook, 3 -> Bishop, 4 -> Knight, 5 -> Pawn (?)
    unsigned color:1;
    unsigned promoted:1 = 0;
    unsigned promoted_to:2;
} t_move;

#define O_MOVE_ORIGIN 0
#define O_MOVE_TARGET 6
#define O_MOVE_TAKEN_FIGURE 12
#define O_MOVE_COLOR 15


bool empty_between(t_board *board, t_move move);
bool is_move_check(t_board *board, t_move move);
bool is_threatened(t_board *board, Position target, bool color);
bool is_move_legal(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter, bool checkBetween);
bool is_move_legal_nocheck(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter, bool checkBetween);
bool is_castle_legal(t_board *board, Position kingPosition, bool color, bool direction);

List<t_move> generate_moves(t_game *game, bool color);

void printMove(t_move move);

bool is_enpassant(t_board *board, t_move *move);
bool is_double_pawn_move(t_board *board, t_move *move);
bool is_castle(t_board *board, t_move *move);

void doMove(t_board* board, t_move* move);
void undoMove(t_board* board, t_move* move);

#endif //KINGOFTHEHILL_KI_MOVE_H
