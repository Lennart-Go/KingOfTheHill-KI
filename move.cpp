#include "move.h"
#include "util.h"


// Define possible move offsets for all pieces with non-linear or conditional moves

// King
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

const Offset MO_KING_CASTLE_SHORT[] {
        Offset {2, 0}  // Short castle to the right
};
const List<Offset> M_KING_CASTLE_SHORT = { (Offset *) &MO_KING_CASTLE_SHORT, 1 };

const Offset MO_KING_CASTLE_LONG[] {
        Offset {-2, 0}
};
const List<Offset> M_KING_CASTLE_LONG = { (Offset *) &MO_KING_CASTLE_LONG, 1 };

// Knight
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

// Pawns
const Offset MO_PAWN_WHITE[] {
        Offset {0, 1}
};
const List<Offset> M_PAWN_WHITE = { (Offset *) &MO_PAWN_WHITE, 1 };

const Offset MO_PAWN_BLACK[] {
        Offset {0, -1}
};
const List<Offset> M_PAWN_BLACK = { (Offset *) &MO_PAWN_BLACK, 1 };

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



bool empty_between(t_board *board, t_move move) {
    /*
     * Function to check whether the "line of sight" between a move's origin and target is clear of obstructions
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  t_move move: Move struct object containing origin, target and color of the moving piece
     * Return:
     *  bool: "true" if the line of sight from origin to target is clear, "false" otherwise
     */

    uint64_t occupied = board->white | board->black;

    Position originPosition = Position(move.origin >> 3, move.origin & 0b111);
    Position targetPosition = Position(move.target >> 3, move.target & 0b111);

    bool xChanged = (originPosition.x == targetPosition.x);
    bool yChanged = (originPosition.y == targetPosition.y);

    int xChange;
    int xDiff = ((int )originPosition.x - (int )targetPosition.x);
    if (xChanged) {
        xChange = xDiff / abs(xDiff);
    } else {
        xChange = 0;
    }

    int yChange;
    int yDiff = ((int )originPosition.y - (int )targetPosition.y);
    if (yChanged) {
        yChange = yDiff / abs(yDiff);
    } else {
        yChange = 0;
    }

    for (int xOffset=xChange, yOffset=yChange; abs(xOffset)<abs(xDiff) && abs(yOffset)<abs(xDiff); xOffset+=xChange, yOffset+=yChange) {
        Offset target = Offset(xOffset, yOffset) + originPosition;

        if (target.isWithinBounds()) {
            int targetShift = shift_from_position(target.toPosition());
            if (board_value_from_shift(occupied, targetShift)) {
                return false;
            }
        }
    }

    return true;
}

bool is_move_legal_nocheck(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter, bool checkBetween) {
    /*
     * Function to check whether a given move is legal, whilst ignoring restrictions from possible checks
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  t_move move: Move struct object containing origin, target and color of the moving piece
     *  uint64_t color_filter: Bitmap containing information where pieces of the moving team are located
     *  uint64_t enemy_color_filter: Bitmap containing information where pieces of the moving team's enemy are located
     *  bool checkBetween: boolean flag, whether to check for obstructions between origin and target of the given move
     * Return:
     *  "true" if the move is legal considering the information passed to the function, "false" otherwise
     */

    // Extract information from move
    Position targetPosition = Position(move.target >> 3, move.target & 0b111);

    int targetPositionShift = shift_from_position(targetPosition);

    // Check if target field is empty or occupied by enemy piece
    if (!board_value_from_shift(~color_filter | enemy_color_filter, targetPositionShift)) {
        // Target is occupied by friendly piece
        return false;
    }

    // Check if path from origin to target is clear
    if (checkBetween && !empty_between(board, move)) {
        // Path from origin to target is blocked by an arbitrary piece
        return false;
    }

    return true;
}

bool is_threatened(t_board *board, Position target, bool color) {
    /*
     * Function to check whether a given position is being threatened by any of the enemy team's pieces
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  Position target: Position object containing the location on the board, that is to be investigated for threats
     *  bool color: Specifies the color of the team whose turn it is. "false" for white, "true" for black
     * Return:
     *  "true" if the specified position is under threat by a piece, "false" otherwise
     */

    uint64_t color_filter, enemy_color_filter;
    List<Offset> M_PAWN, M_PAWN_DOUBLE, M_PAWN_TAKE;
    if (!color) {
        color_filter = board->white;
        enemy_color_filter = board->black;

        M_PAWN_TAKE = M_PAWN_WHITE_TAKE;
    } else {
        color_filter = board->black;
        enemy_color_filter = board->white;

        M_PAWN_TAKE = M_PAWN_BLACK_TAKE;
    }

    // Check for threatening move for all pieces

    // King
    List<Position> kingPositions = board_value_positions(board->king & color_filter);
    if (kingPositions.length() != 1) {
        // Dafuq?
        return false;  // Abort move generation, because no/many king?  // TODO: Throw error?
    } else {
        Position kingPosition = kingPositions.get(0);

        for (int offsetIndex = 0; offsetIndex < M_KING.length(); ++offsetIndex) {
            Offset targetOffset = M_KING.get(offsetIndex) + kingPosition;

            if (targetOffset.isWithinBounds() && (targetOffset == target)) {
                Position moveTarget = targetOffset.toPosition();

                t_move possibleMove;
                possibleMove.origin = (kingPosition.x << 3) | kingPosition.y;
                possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                possibleMove.color = color;

                if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, false)) {
                    return true;
                }
            }
        }
    }

    // Queen
    List<Position> queenPositions = board_value_positions(board->queen & color_filter);
    for (int queenPositionIndex = 0; queenPositionIndex < queenPositions.length(); ++queenPositionIndex) {
        Position queenPosition = queenPositions.get(queenPositionIndex);

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
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_0 = false;
                }
            }
            if (a_45) {
                Offset targetOffset = Offset(distanceOffset, distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_45 = false;
                }
            }
            if (a_90) {
                Offset targetOffset = Offset(distanceOffset, 0) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_90 = false;
                }
            }
            if (a_135) {
                Offset targetOffset = Offset(distanceOffset, -distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_135 = false;
                }
            }
            if (a_180) {
                Offset targetOffset = Offset(0, -distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_180 = false;
                }
            }
            if (a_225) {
                Offset targetOffset = Offset(-distanceOffset, -distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_225 = false;
                }
            }
            if (a_270) {
                Offset targetOffset = Offset(-distanceOffset, 0) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_270 = false;
                }
            }
            if (a_315) {
                Offset targetOffset = Offset(-distanceOffset, distanceOffset) + queenPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (queenPosition.x << 3) | queenPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_315 = false;
                }
            }
        }
    }

    // Rook
    List<Position> rookPositions = board_value_positions(board->rook & color_filter);
    for (int rookPositionIndex = 0; rookPositionIndex < rookPositions.length(); ++rookPositionIndex) {
        Position rookPosition = rookPositions.get(rookPositionIndex);

        bool a_0 = true;
        bool a_90 = true;
        bool a_180 = true;
        bool a_270 = true;

        for (int distanceOffset = 1; distanceOffset < 8; ++distanceOffset) {
            if (a_0) {
                Offset targetOffset = Offset(0, distanceOffset) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (rookPosition.x << 3) | rookPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_0 = false;
                }
            }
            if (a_90) {
                Offset targetOffset = Offset(distanceOffset, 0) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (rookPosition.x << 3) | rookPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_90 = false;
                }
            }
            if (a_180) {
                Offset targetOffset = Offset(0, -distanceOffset) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (rookPosition.x << 3) | rookPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_180 = false;
                }
            }
            if (a_270) {
                Offset targetOffset = Offset(-distanceOffset, 0) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (rookPosition.x << 3) | rookPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_270 = false;
                }
            }
        }
    }

    // Bishop
    List<Position> bishopPositions = board_value_positions(board->bishop & color_filter);
    for (int bishopPositionIndex = 0; bishopPositionIndex < bishopPositions.length(); ++bishopPositionIndex) {
        Position bishopPosition = bishopPositions.get(bishopPositionIndex);

        bool a_45 = true;
        bool a_135 = true;
        bool a_225 = true;
        bool a_315 = true;

        for (int distanceOffset = 1; distanceOffset < 8; ++distanceOffset) {
            if (a_45) {
                Offset targetOffset = Offset(distanceOffset, distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (bishopPosition.x << 3) | bishopPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_45 = false;
                }
            }
            if (a_135) {
                Offset targetOffset = Offset(distanceOffset, -distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (bishopPosition.x << 3) | bishopPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_135 = false;
                }
            }
            if (a_225) {
                Offset targetOffset = Offset(-distanceOffset, -distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (bishopPosition.x << 3) | bishopPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_225 = false;
                }
            }
            if (a_315) {
                Offset targetOffset = Offset(-distanceOffset, distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    if (targetOffset == target) {
                        Position moveTarget = targetOffset.toPosition();

                        t_move possibleMove;
                        possibleMove.origin = (bishopPosition.x << 3) | bishopPosition.y;
                        possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                        possibleMove.color = color;

                        if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                            return true;
                        }
                    }
                } else {
                    a_315 = false;
                }
            }
        }
    }

    // Knight
    List<Position> knightPositions = board_value_positions(board->knight & color_filter);
    for (int knightPositionIndex = 0; knightPositionIndex < knightPositions.length(); ++knightPositionIndex) {
        Position knightPosition = knightPositions.get(knightPositionIndex);

        for (int offsetIndex = 0; offsetIndex < M_KNIGHT.length(); ++offsetIndex) {
            Offset targetOffset = M_KNIGHT.get(offsetIndex) + knightPosition;

            if (targetOffset.isWithinBounds() && (targetOffset == target)) {
                Position moveTarget = targetOffset.toPosition();

                t_move possibleMove;
                possibleMove.origin = (knightPosition.x << 3) | knightPosition.y;
                possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                possibleMove.color = color;

                if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, false)) {
                    return true;
                }
            }
        }
    }

    // Pawns
    List<Position> pawnPositions = board_value_positions(board->pawn & color_filter);
    for (int pawnPositionIndex = 0; pawnPositionIndex < pawnPositions.length(); ++pawnPositionIndex) {
        Position pawnPosition = pawnPositions.get(pawnPositionIndex);

        // Taking move
        for (int offsetIndex = 0; offsetIndex < M_PAWN_TAKE.length(); ++offsetIndex) {
            Offset targetOffset = M_PAWN_TAKE.get(offsetIndex) + pawnPosition;

            if (targetOffset.isWithinBounds() && (targetOffset == target)) {
                Position moveTarget = targetOffset.toPosition();

                t_move possibleMove;
                possibleMove.origin = (pawnPosition.x << 3) | pawnPosition.y;
                possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
                possibleMove.color = color;

                if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, false)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool is_check(t_board *board, t_move move) {
    /*
     * Function to check whether taking a given move would result in a check against the moving team
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  t_move move: Move struct object containing origin, target and color of the moving piece
     * Return:
     *  "true" if the move is causing the moving team's king to be under attack, "false" otherwise
     */

    bool color = (bool ) move.color;
    bool retValue = false;

    // TODO: Apply move to board before checking if the king is threatened
    doMove(board, &move);

    // Get position of king
    uint64_t color_mask;
    if (!color) {
        color_mask = board->white;
    } else {
        color_mask = board->black;
    }

    List<Position> kingPositions = board_value_positions(board->king & color_mask);
    if (kingPositions.length() != 1) {
        // Should not be possible to have no or multiple kings on the board
        return false;
    } else {
        if (is_threatened(board, kingPositions.get(0), !color)) {
            retValue = true;
        }
    }

    undoMove(board, &move);

    return retValue;
}

bool is_move_legal(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter, bool checkBetween) {
    /*
     * Function to check whether a given move is legal
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  t_move move: Move struct object containing origin, target and color of the moving piece
     *  uint64_t color_filter: Bitmap containing information where pieces of the moving team are located
     *  uint64_t enemy_color_filter: Bitmap containing information where pieces of the moving team's enemy are located
     *  bool checkBetween: boolean flag, whether to check for obstructions between origin and target of the given move
     * Return:
     *  "true" if the move is legal considering the information passed to the function, "false" otherwise
     */

    // Extract information from move
    Position targetPosition = Position(move.target >> 3, move.target & 0b111);

    int targetPositionShift = shift_from_position(targetPosition);

    // Check if target field is empty or occupied by enemy piece
    if (!board_value_from_shift(~color_filter | enemy_color_filter, targetPositionShift)) {
        // Target is occupied by friendly piece
        return false;
    }

    // Check if path from origin to target is clear
    if (checkBetween && !empty_between(board, move)) {
        // Path from origin to target is blocked by an arbitrary piece
        return false;
    }

    // Check if move taken would result in a check
    if (is_check(board, move)) {
        return false;
    }

    return true;
}


/*
 * TODO: Ideas
 * - Check if move is legal during move generation and cancel any further generations in that "direction" if failing
 *      -> For pieces with continuously generated moves (Queen, Rook, Bishop)
 */


List<t_move> generate_moves(t_board *board, bool color) {
    /*
     * Function that generates a list of all legal moves in the current state of the game
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  bool color: Specifies the color of the team whose turn it is. "false" for white, "true" for black
     * Return:
     *  List<t_move>: A list of t_move objects containing information on origin, target and color of the legal moves
     */

    List<t_move> moves = List<t_move>();

    uint64_t color_filter, enemy_color_filter;
    List<Offset> M_PAWN, M_PAWN_DOUBLE, M_PAWN_TAKE;
    if (!color) {
        color_filter = board->white;
        enemy_color_filter = board->black;

        M_PAWN = M_PAWN_WHITE;
        M_PAWN_DOUBLE = M_PAWN_WHITE_DOUBLE;
        M_PAWN_TAKE = M_PAWN_WHITE_TAKE;
    } else {
        color_filter = board->black;
        enemy_color_filter = board->white;

        M_PAWN = M_PAWN_BLACK;
        M_PAWN_DOUBLE = M_PAWN_BLACK_DOUBLE;
        M_PAWN_TAKE = M_PAWN_BLACK_TAKE;
    }

    // Generate moves for all pieces

    // TODO: Castling
    // King
    List<Position> kingPositions = board_value_positions(board->king & color_filter);
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
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, false)) {
                moves.add(possibleMove);
            }
        }
    }

    // Queen
    List<Position> queenPositions = board_value_positions(board->queen & color_filter);
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
                    a_180 = false;
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
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, true)) {
                moves.add(possibleMove);
            }
        }
    }

    // Rook
    List<Position> rookPositions = board_value_positions(board->rook & color_filter);
    for (int rookPositionIndex = 0; rookPositionIndex < rookPositions.length(); ++rookPositionIndex) {
        Position rookPosition = rookPositions.get(rookPositionIndex);
        List<Position> possibleTargets = List<Position>();

        bool a_0 = true;
        bool a_90 = true;
        bool a_180 = true;
        bool a_270 = true;

        for (int distanceOffset = 1; distanceOffset < 8; ++distanceOffset) {
            if (a_0) {
                Offset targetOffset = Offset(0, distanceOffset) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_0 = false;
                }
            }
            if (a_90) {
                Offset targetOffset = Offset(distanceOffset, 0) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_90 = false;
                }
            }
            if (a_180) {
                Offset targetOffset = Offset(0, -distanceOffset) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_180 = false;
                }
            }
            if (a_270) {
                Offset targetOffset = Offset(-distanceOffset, 0) + rookPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_270 = false;
                }
            }
        }

        for (int targetIndex = 0; targetIndex < possibleTargets.length(); ++targetIndex) {
            Position moveTarget = possibleTargets.get(targetIndex);

            t_move possibleMove;
            possibleMove.origin = (rookPosition.x << 3) | rookPosition.y;
            possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, true)) {
                moves.add(possibleMove);
            }
        }
    }

    // Bishop
    List<Position> bishopPositions = board_value_positions(board->bishop & color_filter);
    for (int bishopPositionIndex = 0; bishopPositionIndex < bishopPositions.length(); ++bishopPositionIndex) {
        Position bishopPosition = bishopPositions.get(bishopPositionIndex);
        List<Position> possibleTargets = List<Position>();

        bool a_45 = true;
        bool a_135 = true;
        bool a_225 = true;
        bool a_315 = true;

        for (int distanceOffset = 1; distanceOffset < 8; ++distanceOffset) {
            if (a_45) {
                Offset targetOffset = Offset(distanceOffset, distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_45 = false;
                }
            }
            if (a_135) {
                Offset targetOffset = Offset(distanceOffset, -distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_135 = false;
                }
            }
            if (a_225) {
                Offset targetOffset = Offset(-distanceOffset, -distanceOffset) + bishopPosition;

                if (targetOffset.isWithinBounds()) {
                    possibleTargets.add(targetOffset.toPosition());
                } else {
                    a_225 = false;
                }
            }
            if (a_315) {
                Offset targetOffset = Offset(-distanceOffset, distanceOffset) + bishopPosition;

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
            possibleMove.origin = (bishopPosition.x << 3) | bishopPosition.y;
            possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, true)) {
                moves.add(possibleMove);
            }
        }
    }

    // Knight
    List<Position> knightPositions = board_value_positions(board->knight & color_filter);
    for (int knightPositionIndex = 0; knightPositionIndex < knightPositions.length(); ++knightPositionIndex) {
        Position knightPosition = knightPositions.get(knightPositionIndex);
        List<Position> possibleTargets = List<Position>();

        for (int offsetIndex = 0; offsetIndex < M_KNIGHT.length(); ++offsetIndex) {
            Offset targetOffset = M_KNIGHT.get(offsetIndex) + knightPosition;

            if (targetOffset.isWithinBounds()) {
                possibleTargets.add(targetOffset.toPosition());
            }
        }

        for (int targetIndex = 0; targetIndex < possibleTargets.length(); ++targetIndex) {
            Position moveTarget = possibleTargets.get(targetIndex);

            t_move possibleMove;
            possibleMove.origin = (knightPosition.x << 3) | knightPosition.y;
            possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, false)) {
                moves.add(possibleMove);
            }
        }
    }

    // TODO: Promotions and En-Passant?
    // Pawns
    List<Position> pawnPositions = board_value_positions(board->pawn & color_filter);
    for (int pawnPositionIndex = 0; pawnPositionIndex < pawnPositions.length(); ++pawnPositionIndex) {
        Position pawnPosition = pawnPositions.get(pawnPositionIndex);
        List<Position> possibleTargets = List<Position>();

        // "Normal" forward move
        Offset targetOffset = M_PAWN.get(0) + pawnPosition;
        if (targetOffset.isWithinBounds()) {
            // Should not be possible to not be within bounds
            int targetOffsetShift = shift_from_position(targetOffset.toPosition());
            bool isTargetOccupied = board_value_from_shift(color_filter | enemy_color_filter, targetOffsetShift);

            if (!isTargetOccupied) {
                possibleTargets.add(targetOffset.toPosition());
            }
        }

        // Double forward move
        if (pawnPosition.y == 1) {
            // Only possible when pawn has not moved yet
            targetOffset = M_PAWN_DOUBLE.get(0) + pawnPosition;
            if (targetOffset.isWithinBounds()) {
                // Should not be possible to not be within bounds
                int targetOffsetShift = shift_from_position(targetOffset.toPosition());
                bool isTargetOccupied = board_value_from_shift(color_filter | enemy_color_filter, targetOffsetShift);

                if (!isTargetOccupied) {
                    possibleTargets.add(targetOffset.toPosition());
                }
            }
        }

        // Taking move
        for (int offsetIndex = 0; offsetIndex < M_PAWN_TAKE.length(); ++offsetIndex) {
            targetOffset = M_PAWN_TAKE.get(offsetIndex) + pawnPosition;

            if (targetOffset.isWithinBounds()) {
                int targetOffsetShift = shift_from_position(targetOffset.toPosition());
                bool isTargetOccupiedByEnemy = board_value_from_shift(enemy_color_filter, targetOffsetShift);

                if (isTargetOccupiedByEnemy) {
                    possibleTargets.add(targetOffset.toPosition());
                }
            }
        }

        for (int targetIndex = 0; targetIndex < possibleTargets.length(); ++targetIndex) {
            Position moveTarget = possibleTargets.get(targetIndex);

            t_move possibleMove;
            possibleMove.origin = (pawnPosition.x << 3) | pawnPosition.y;
            possibleMove.target = (moveTarget.x << 3) | moveTarget.y;
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, true)) {
                moves.add(possibleMove);
            }
        }
    }

    return moves;
}
