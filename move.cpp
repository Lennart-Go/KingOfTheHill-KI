#include "move.h"
#include "util.h"


// Define possible move offsets for all pieces
const Offset MO_KING[]{
        Offset {0,  1},  // Up
        Offset {1,  1},  // Up & Right
        Offset {1,  0},  // Right
        Offset {1,  -1},  // Down & Right
        Offset {0,  -1},  // Down
        Offset {-1, -1},  // Down & Left
        Offset {-1, 0},  // Left
        Offset {-1, 1},  // Up & Left
};
const List<Offset> M_KING { (Offset *) &MO_KING,8 };

//const Offset MO_QUEEN[]{
//        Offset {1,  0},  // Right
//        Offset {2,  0},
//        Offset {3,  0},
//        Offset {4,  0},
//        Offset {5,  0},
//        Offset {6,  0},
//        Offset {7,  0},
//        Offset {-1, 0},  // Left
//        Offset {-2, 0},
//        Offset {-3, 0},
//        Offset {-4, 0},
//        Offset {-5, 0},
//        Offset {-6, 0},
//        Offset {-7, 0},
//        Offset {0,  1},  // Up
//        Offset {0,  2},
//        Offset {0,  3},
//        Offset {0,  4},
//        Offset {0,  5},
//        Offset {0,  6},
//        Offset {0,  7},
//        Offset {0,  -1},  // Down
//        Offset {0,  -2},
//        Offset {0,  -3},
//        Offset {0,  -4},
//        Offset {0,  -5},
//        Offset {0,  -6},
//        Offset {0,  -7},
//        Offset {1,  1},  // Up & Right
//        Offset {2,  2},
//        Offset {3,  3},
//        Offset {4,  4},
//        Offset {5,  5},
//        Offset {6,  6},
//        Offset {7,  7},
//        Offset {1,  -1},  // Down & Right
//        Offset {2,  -2},
//        Offset {3,  -3},
//        Offset {4,  -4},
//        Offset {5,  -5},
//        Offset {6,  -6},
//        Offset {7,  -7},
//        Offset {-1, 1},  // Up & Left
//        Offset {-2, 2},
//        Offset {-3, 3},
//        Offset {-4, 4},
//        Offset {-5, 5},
//        Offset {-6, 6},
//        Offset {-7, 7},
//        Offset {-1, -1},  // Down & Left
//        Offset {-2, -2},
//        Offset {-3, -3},
//        Offset {-4, -4},
//        Offset {-5, -5},
//        Offset {-6, -6},
//        Offset {-7, -7},
//
//};
//const List<Offset> M_QUEEN{ (Offset *) &MO_QUEEN, 56 };
//
//const Offset MO_ROOK[]{
//        Offset {1,  0},  // Right
//        Offset {2,  0},
//        Offset {3,  0},
//        Offset {4,  0},
//        Offset {5,  0},
//        Offset {6,  0},
//        Offset {7,  0},
//        Offset {-1, 0},  // Left
//        Offset {-2, 0},
//        Offset {-3, 0},
//        Offset {-4, 0},
//        Offset {-5, 0},
//        Offset {-6, 0},
//        Offset {-7, 0},
//        Offset {0,  1},  // Up
//        Offset {0,  2},
//        Offset {0,  3},
//        Offset {0,  4},
//        Offset {0,  5},
//        Offset {0,  6},
//        Offset {0,  7},
//        Offset {0,  -1},  // Down
//        Offset {0,  -2},
//        Offset {0,  -3},
//        Offset {0,  -4},
//        Offset {0,  -5},
//        Offset {0,  -6},
//        Offset {0,  -7},
//};
//const List<Offset> M_ROOK = { (Offset *) &MO_ROOK, 28 };
//
//const Offset MO_BISHOP[] {
//        Offset {1,  1},  // Up & Right
//        Offset {2,  2},
//        Offset {3,  3},
//        Offset {4,  4},
//        Offset {5,  5},
//        Offset {6,  6},
//        Offset {7,  7},
//        Offset {1,  -1},  // Down & Right
//        Offset {2,  -2},
//        Offset {3,  -3},
//        Offset {4,  -4},
//        Offset {5,  -5},
//        Offset {6,  -6},
//        Offset {7,  -7},
//        Offset {-1, 1},  // Up & Left
//        Offset {-2, 2},
//        Offset {-3, 3},
//        Offset {-4, 4},
//        Offset {-5, 5},
//        Offset {-6, 6},
//        Offset {-7, 7},
//        Offset {-1, -1},  // Down & Left
//        Offset {-2, -2},
//        Offset {-3, -3},
//        Offset {-4, -4},
//        Offset {-5, -5},
//        Offset {-6, -6},
//        Offset {-7, -7},
//};
//const List<Offset> M_BISHOP = { (Offset *) &MO_BISHOP, 28 };

const Offset MO_KNIGHT[] {
        Offset {1, 2},  // Up & Right
        Offset {2, 1},  // Right & Up
        Offset {2, -1},  // Right & Down
        Offset {1, -2},  // Down & Right
        Offset {-1, -2},  // Down & Left
        Offset {-2, -1},  // Left & Down
        Offset {-2, 1},  // Left & Up
        Offset {-1, 2},  // Up & Left
};
const List<Offset> M_KNIGHT = { (Offset *) &MO_KNIGHT, 8 };

const Offset MO_PAWN_WHITE[] {
        Offset {0, 1}
};
const List<Offset> M_PAWN_WHITE = { (Offset *) &MO_PAWN_WHITE, 1 };

const Offset MO_PAWN_BLACK[] {
        Offset {0, -1}
};
const List<Offset> M_PAWN_BLACK = { (Offset *) &MO_PAWN_WHITE, 1 };


// Special moves
const Offset MO_KING_CASTLE_SHORT[] {
        Offset {2, 0}  // Short castle to the right
};
const List<Offset> M_KING_CASTLE_SHORT = { (Offset *) &MO_KING_CASTLE_SHORT, 1 };

const Offset MO_KING_CASTLE_LONG[] {
        Offset {-2, 0}
};
const List<Offset> M_KING_CASTLE_LONG = { (Offset *) &MO_KING_CASTLE_LONG, 1 };

const Offset MO_PAWN_WHITE_TAKE[] {
        Offset {1, 1},  // Take diagonal right
        Offset {-1, 1},  // Take diagonal left
};
const List<Offset> M_PAWN_WHITE_TAKE = { (Offset *) &MO_PAWN_WHITE_TAKE, 2 };

const Offset MO_PAWN_WHITE_DOUBLE[] {
        Offset {0, 2}
};
const List<Offset> M_PAWN_WHITE_DOUBLE = { (Offset *) &MO_PAWN_WHITE_DOUBLE, 1 };

const Offset MO_PAWN_BLACK_TAKE[] {
        Offset {1, -1},  // Take diagonal right
        Offset {-1, -1},  // Take diagonal left
};
const List<Offset>  M_PAWN_BLACK_TAKE = { (Offset *) &MO_PAWN_BLACK_TAKE, 2 };

const Offset MO_PAWN_BLACK_DOUBLE[] {
        Offset {0, -2}
};
const List<Offset> M_PAWN_BLACK_DOUBLE = { (Offset *) &MO_PAWN_BLACK_DOUBLE, 1 };



bool is_move_legal(t_board board, t_move move) {
    // TODO: Actually Check if move is legal
    return true;
}


List<t_move> generate_moves(t_board board, int color) {
    List<t_move> moves = List<t_move>();

    uint64_t color_filter;
    if (color == 0) {
        color_filter = board.white;
    } else {
        color_filter = board.black;
    }

    // Generate moves for all pieces
    // King
    List<Position> kingPositions = board_value_positions(board.king & color_filter);
    if (kingPositions.length() != 1) {
        // Dafuq?
        return moves;  // Abort move generation, because no/many king?
    } else {
        Position kingPosition = kingPositions.get(0);
        List<Position> possibleTargets = List<Position>();

        for (int offsetIndex = 0; offsetIndex < M_KING.length(); ++offsetIndex) {
            Offset targetOffset = M_KING.get(offsetIndex) + kingPosition;

            if (targetOffset.isWithinBounds()) {
                possibleTargets.add(targetOffset.toPosition());
            }
        }

        for (int targetIndex = 0; targetIndex < possibleTargets.length(); ++targetIndex) {
            Position moveTarget = possibleTargets.get(targetIndex);

            t_move possibleMove;
            possibleMove.origin = (kingPosition.x << 3) | kingPosition.y;
            possibleMove.target = (moveTarget.x << 3) | moveTarget.y;

            if (is_move_legal(board, possibleMove)) {
                moves.add(possibleMove);
            }
        }
    }

    // TODO: Queen, Rook, Bishop, Knight, Pawn
    // Queen
    List<Position> queenPositions = board_value_positions(board.queen & color_filter);
    for (int queenPositionIndex = 0; queenPositionIndex < queenPositions.length(); ++queenPositionIndex) {
        Position queenPosition = queenPositions.get(queenPositionIndex);
        List<Position> possibleTargets = List<Position>();

        bool a_0 = true;
        bool a_45 = true;
        bool a_90 = true;
        bool a_135 = true;
        bool a_180 = true;
        bool a_225 = true;
        bool a_270 = true;
        bool a_315 = true;

        for (int distanceOffset = 1; distanceOffset < 8; ++distanceOffset) {
            if (a_0) {
                Offset targetOffset = Offset(0, distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_0 = false;
                }
            }
            if (a_45) {
                Offset targetOffset = Offset(distanceOffset, distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_45 = false;
                }
            }
            if (a_90) {
                Offset targetOffset = Offset(distanceOffset, 0) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_90 = false;
                }
            }
            if (a_135) {
                Offset targetOffset = Offset(distanceOffset, -distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_135 = false;
                }
            }
            if (a_180) {
                Offset targetOffset = Offset(0, -distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_0 = false;
                }
            }
            if (a_225) {
                Offset targetOffset = Offset(-distanceOffset, -distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_225 = false;
                }
            }
            if (a_270) {
                Offset targetOffset = Offset(-distanceOffset, 0) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_270 = false;
                }
            }
            if (a_315) {
                Offset targetOffset = Offset(-distanceOffset, distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_315 = false;
                }
            }
        }

        for (int targetIndex = 0; targetIndex < possibleTargets.length(); ++targetIndex) {
            Position moveTarget = possibleTargets.get(targetIndex);

            t_move possibleMove;
            possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
            possibleMove.target = (moveTarget.x << 3) | moveTarget.y;

            if (is_move_legal(board, possibleMove)) {
                moves.add(possibleMove);
            }
        }
    }
}
