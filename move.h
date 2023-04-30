#include "util.h"
#include "board.h"

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
