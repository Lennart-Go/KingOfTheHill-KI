#include "util.h"
#include "board.h"

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
    unsigned taken_figure:3;
    unsigned color:1;
} t_move;

#define O_MOVE_ORIGIN 0
#define O_MOVE_TARGET 6
#define O_MOVE_TAKEN_FIGURE 12
#define O_MOVE_COLOR 15


bool empty_between(t_board *board, t_move move);
bool is_check(t_board *board, t_move move);
bool is_threatened(t_board *board, Position target, bool color);
bool is_move_legal(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter, bool checkBetween);
bool is_move_legal_nocheck(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter, bool checkBetween);

List<t_move> generate_moves(t_board *board, bool color);

#endif //KINGOFTHEHILL_KI_MOVE_H
