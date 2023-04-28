#include "move.h"


// Define possible move offsets for all pieces
const t_offset MO_KING[]{
        t_offset {0,  1},  // Up
        t_offset {1,  1},  // Up & Right
        t_offset {1,  0},  // Right
        t_offset {1,  -1},  // Down & Right
        t_offset {0,  -1},  // Down
        t_offset {-1, -1},  // Down & Left
        t_offset {-1, 0},  // Left
        t_offset {-1, 1},  // Up & Left
};
const t_countedList M_KING = { .list = (void *) &MO_KING, .count = 8 };

const t_offset MO_QUEEN[]{
        t_offset {1,  0},  // Right
        t_offset {2,  0},
        t_offset {3,  0},
        t_offset {4,  0},
        t_offset {5,  0},
        t_offset {6,  0},
        t_offset {7,  0},
        t_offset {-1, 0},  // Left
        t_offset {-2, 0},
        t_offset {-3, 0},
        t_offset {-4, 0},
        t_offset {-5, 0},
        t_offset {-6, 0},
        t_offset {-7, 0},
        t_offset {0,  1},  // Up
        t_offset {0,  2},
        t_offset {0,  3},
        t_offset {0,  4},
        t_offset {0,  5},
        t_offset {0,  6},
        t_offset {0,  7},
        t_offset {0,  -1},  // Down
        t_offset {0,  -2},
        t_offset {0,  -3},
        t_offset {0,  -4},
        t_offset {0,  -5},
        t_offset {0,  -6},
        t_offset {0,  -7},
        t_offset {1,  1},  // Up & Right
        t_offset {2,  2},
        t_offset {3,  3},
        t_offset {4,  4},
        t_offset {5,  5},
        t_offset {6,  6},
        t_offset {7,  7},
        t_offset {1,  -1},  // Down & Right
        t_offset {2,  -2},
        t_offset {3,  -3},
        t_offset {4,  -4},
        t_offset {5,  -5},
        t_offset {6,  -6},
        t_offset {7,  -7},
        t_offset {-1, 1},  // Up & Left
        t_offset {-2, 2},
        t_offset {-3, 3},
        t_offset {-4, 4},
        t_offset {-5, 5},
        t_offset {-6, 6},
        t_offset {-7, 7},
        t_offset {-1, -1},  // Down & Left
        t_offset {-2, -2},
        t_offset {-3, -3},
        t_offset {-4, -4},
        t_offset {-5, -5},
        t_offset {-6, -6},
        t_offset {-7, -7},

};
const t_countedList M_QUEEN{ .list = (void *) &MO_QUEEN, .count = 56 };

const t_offset MO_ROOK[]{
        t_offset {1,  0},  // Right
        t_offset {2,  0},
        t_offset {3,  0},
        t_offset {4,  0},
        t_offset {5,  0},
        t_offset {6,  0},
        t_offset {7,  0},
        t_offset {-1, 0},  // Left
        t_offset {-2, 0},
        t_offset {-3, 0},
        t_offset {-4, 0},
        t_offset {-5, 0},
        t_offset {-6, 0},
        t_offset {-7, 0},
        t_offset {0,  1},  // Up
        t_offset {0,  2},
        t_offset {0,  3},
        t_offset {0,  4},
        t_offset {0,  5},
        t_offset {0,  6},
        t_offset {0,  7},
        t_offset {0,  -1},  // Down
        t_offset {0,  -2},
        t_offset {0,  -3},
        t_offset {0,  -4},
        t_offset {0,  -5},
        t_offset {0,  -6},
        t_offset {0,  -7},
};
const t_countedList M_ROOK = { .list = (void *) &MO_ROOK, .count = 28 };

const t_offset MO_BISHOP[] {
        t_offset {1,  1},  // Up & Right
        t_offset {2,  2},
        t_offset {3,  3},
        t_offset {4,  4},
        t_offset {5,  5},
        t_offset {6,  6},
        t_offset {7,  7},
        t_offset {1,  -1},  // Down & Right
        t_offset {2,  -2},
        t_offset {3,  -3},
        t_offset {4,  -4},
        t_offset {5,  -5},
        t_offset {6,  -6},
        t_offset {7,  -7},
        t_offset {-1, 1},  // Up & Left
        t_offset {-2, 2},
        t_offset {-3, 3},
        t_offset {-4, 4},
        t_offset {-5, 5},
        t_offset {-6, 6},
        t_offset {-7, 7},
        t_offset {-1, -1},  // Down & Left
        t_offset {-2, -2},
        t_offset {-3, -3},
        t_offset {-4, -4},
        t_offset {-5, -5},
        t_offset {-6, -6},
        t_offset {-7, -7},
};
const t_countedList M_BISHOP = { .list = (void *) &MO_BISHOP, .count = 28 };

const t_offset MO_KNIGHT[] {
        t_offset {1, 2},  // Up & Right
        t_offset {2, 1},  // Right & Up
        t_offset {2, -1},  // Right & Down
        t_offset {1, -2},  // Down & Right
        t_offset {-1, -2},  // Down & Left
        t_offset {-2, -1},  // Left & Down
        t_offset {-2, 1},  // Left & Up
        t_offset {-1, 2},  // Up & Left
};
const t_countedList M_KNIGHT = { .list = (void *) &MO_KNIGHT, .count = 8 };

const t_offset MO_PAWN_WHITE[] {
        t_offset {0, 1}
};
const t_countedList M_PAWN_WHITE = { .list = (void *) &MO_PAWN_WHITE, .count = 1 };

const t_offset MO_PAWN_BLACK[] {
        t_offset {0, -1}
};
const t_countedList M_PAWN_BLACK = { .list = (void *) &MO_PAWN_WHITE, .count = 1 };


// Special moves
const t_offset MO_KING_CASTLE_SHORT[] {
        t_offset {2, 0}  // Short castle to the right
};
const t_countedList M_KING_CASTLE_SHORT = { .list = (void *) &MO_KING_CASTLE_SHORT, .count = 1 };

const t_offset MO_KING_CASTLE_LONG[] {
        t_offset {-2, 0}
};
const t_countedList M_KING_CASTLE_LONG = { .list = (void *) &MO_KING_CASTLE_LONG, .count = 1 };

const t_offset MO_PAWN_WHITE_TAKE[] {
        t_offset {1, 1},  // Take diagonal right
        t_offset {-1, 1},  // Take diagonal left
};
const t_countedList M_PAWN_WHITE_TAKE = { .list = (void *) &MO_PAWN_WHITE_TAKE, .count = 2 };

const t_offset MO_PAWN_WHITE_DOUBLE[] {
        t_offset {0, 2}
};
const t_countedList M_PAWN_WHITE_DOUBLE = { .list = (void *) &MO_PAWN_WHITE_DOUBLE, .count = 1 };

const t_offset MO_PAWN_BLACK_TAKE[] {
        t_offset {1, -1},  // Take diagonal right
        t_offset {-1, -1},  // Take diagonal left
};
const t_countedList  M_PAWN_BLACK_TAKE = { .list = (void *) &MO_PAWN_BLACK_TAKE, .count = 2 };

const t_offset MO_PAWN_BLACK_DOUBLE[] {
        t_offset {0, -2}
};
const t_countedList M_PAWN_BLACK_DOUBLE = { .list = (void *) &MO_PAWN_BLACK_DOUBLE, .count = 1 };



// TODO: Generate possible moves
// TODO: Check if move is legal

