#include <cmath>
#include "move.h"
#include "board.h"
#include "game.h"


// Define possible move offsets for all pieces with non-linear or conditional moves

// King
const Offset MO_KING[]{
        Offset{0, 1},  // Up
        Offset{1, 1},  // Up & Right
        Offset{1, 0},  // Right
        Offset{1, -1},  // Down & Right
        Offset{0, -1},  // Down
        Offset{-1, -1},  // Down & Left
        Offset{-1, 0},  // Left
        Offset{-1, 1},  // Up & Left
};
const List<Offset> M_KING{(Offset *) &MO_KING, 8};

const Offset MO_KING_CASTLE_SHORT[]{
        Offset{2, 0}  // Short castle to the right
};
const List<Offset> M_KING_CASTLE_SHORT = {(Offset *) &MO_KING_CASTLE_SHORT, 1};

const Offset MO_KING_CASTLE_LONG[]{
        Offset{-2, 0}
};
const List<Offset> M_KING_CASTLE_LONG = {(Offset *) &MO_KING_CASTLE_LONG, 1};

// Knight
const Offset MO_KNIGHT[]{
        Offset{1, 2},  // Up & Right
        Offset{2, 1},  // Right & Up
        Offset{2, -1},  // Right & Down
        Offset{1, -2},  // Down & Right
        Offset{-1, -2},  // Down & Left
        Offset{-2, -1},  // Left & Down
        Offset{-2, 1},  // Left & Up
        Offset{-1, 2},  // Up & Left
};
const List<Offset> M_KNIGHT = {(Offset *) &MO_KNIGHT, 8};

// Pawns
const Offset MO_PAWN_WHITE[]{
        Offset{0, 1}
};
const List<Offset> M_PAWN_WHITE = {(Offset *) &MO_PAWN_WHITE, 1};

const Offset MO_PAWN_BLACK[]{
        Offset{0, -1}
};
const List<Offset> M_PAWN_BLACK = {(Offset *) &MO_PAWN_BLACK, 1};

const Offset MO_PAWN_WHITE_TAKE[]{
        Offset{1, 1},  // Take diagonal right
        Offset{-1, 1},  // Take diagonal left
};
const List<Offset> M_PAWN_WHITE_TAKE = {(Offset *) &MO_PAWN_WHITE_TAKE, 2};

const Offset MO_PAWN_WHITE_DOUBLE[]{
        Offset{0, 2}
};
const List<Offset> M_PAWN_WHITE_DOUBLE = {(Offset *) &MO_PAWN_WHITE_DOUBLE, 1};

const Offset MO_PAWN_BLACK_TAKE[]{
        Offset{1, -1},  // Take diagonal right
        Offset{-1, -1},  // Take diagonal left
};
const List<Offset> M_PAWN_BLACK_TAKE = {(Offset *) &MO_PAWN_BLACK_TAKE, 2};

const Offset MO_PAWN_BLACK_DOUBLE[]{
        Offset{0, -2}
};

const List<Offset> M_PAWN_BLACK_DOUBLE = {(Offset *) &MO_PAWN_BLACK_DOUBLE, 1};

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

    Position originPosition = position_from_shift(move.origin);
    Position targetPosition = position_from_shift(move.target);

    bool xChanged = (originPosition.x != targetPosition.x);
    bool yChanged = (originPosition.y != targetPosition.y);

    int xChange;
    int xDiff = ((int) targetPosition.x - (int) originPosition.x);
    if (xChanged) {
        xChange = xDiff / abs(xDiff);
    } else {
        xChange = 0;
        xDiff = 1;
    }

    int yChange;
    int yDiff = ((int) targetPosition.y - (int) originPosition.y);
    if (yChanged) {
        yChange = yDiff / abs(yDiff);
    } else {
        yChange = 0;
        yDiff = 1;
    }

    for (int xOffset = xChange, yOffset = yChange;
         abs(xOffset) < abs(xDiff) && abs(yOffset) < abs(yDiff); xOffset += xChange, yOffset += yChange) {
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

bool is_move_legal_nocheck(t_board *board, t_move move, uint64_t color_filter, uint64_t enemy_color_filter,
                           bool checkBetween) {
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
    Position targetPosition = position_from_shift(move.target);

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
     *  bool color: Specifies the color of the team that may be threatened. "false" for white, "true" for black
     * Return:
     *  "true" if the specified position is under threat by a piece, "false" otherwise
     */

    uint64_t color_filter, enemy_color_filter;
    List<Offset> M_PAWN, M_PAWN_DOUBLE, M_PAWN_TAKE;
    
    if (color) {
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
                possibleMove.origin = shift_from_position(kingPosition);
                possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(queenPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(rookPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(rookPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(rookPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(rookPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(bishopPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(bishopPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(bishopPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                        possibleMove.origin = shift_from_position(bishopPosition);
                        possibleMove.target = shift_from_position(moveTarget);
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
                possibleMove.origin = shift_from_position(knightPosition);
                possibleMove.target = shift_from_position(moveTarget);
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
                possibleMove.origin = shift_from_position(pawnPosition);
                possibleMove.target = shift_from_position(moveTarget);
                possibleMove.color = !color;

                if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, false)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool is_move_check(t_board *board, t_move move) {
    /*
     * Function to check whether taking a given move would result in a check against the moving team
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  t_move move: Move struct object containing origin, target and color of the moving piece
     * Return:
     *  "true" if the move is causing the moving team's king to be under attack, "false" otherwise
     */

    bool color = (bool) move.color;
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
        retValue = false;
    } else {
        if (is_threatened(board, kingPositions.get(0), color)) {
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

    // Check if target field is empty or occupied by enemy piece
    if (!board_value_from_shift(~color_filter | enemy_color_filter, move.target)) {
        // Target is occupied by friendly piece
        return false;
    }

    // Check if path from origin to target is clear
    if (checkBetween && !empty_between(board, move)) {
        // Path from origin to target is blocked by an arbitrary piece
        return false;
    }

    // Check if move taken would result in a check
    if (is_move_check(board, move)) {
        return false;
    }

    return true;
}

bool is_castle_legal(t_board *board, Position kingPosition, bool color, bool direction) {
    /*
     * Function to check whether a castling move in the specified direction is legal. This assumes, that the King is in
     * the correct position on the board, if not unexpected behaviour may arise.
     *
     * Arguments:
     *  t_board *board: Pointer to the board representing the state of the game
     *  Position kingPosition: Position object containing the King's location on the board
     *  bool color: Specifies the color of the team that performs the castling move. "false" for white, "true" for black
     *  bool direction: Specifies whether to examine a short or long castle. "true" for short, "false" for long.
     * Return:
     *  "true" if the casting move is legal
     */

    int directionModifier = -1 + (direction * 2);

    if (is_threatened(board, kingPosition, color)) {
        return false;
    }
    if (is_threatened(board, (Offset(directionModifier * 1, 0) + kingPosition).toPosition(), color)) {
        return false;
    }
    if (is_threatened(board, (Offset(directionModifier * 2, 0) + kingPosition).toPosition(), color)) {
        return false;
    }

    return true;
}


List<t_move> generate_moves(t_game *game, bool color) {
    /*
     * Function that generates a list of all legal moves in the current state of the game
     *
     * Arguments:
     *  t_game *game: Pointer to the game instance representing the state of the game
     * Return:
     *  List<t_move>: A list of t_move objects containing information on origin, target and color of the legal moves
     */

    t_board *board = game->board;

    List<t_move> moves = List<t_move>();

    uint64_t color_filter, enemy_color_filter;
    List<Offset> M_PAWN, M_PAWN_DOUBLE, M_PAWN_TAKE;
    bool canCastleShort, canCastleLong;
    if (!color) {
        color_filter = board->white;
        enemy_color_filter = board->black;

        M_PAWN = M_PAWN_WHITE;
        M_PAWN_DOUBLE = M_PAWN_WHITE_DOUBLE;
        M_PAWN_TAKE = M_PAWN_WHITE_TAKE;

        canCastleShort = game->whiteCanCastleShort;
        canCastleLong = game->whiteCanCastleLong;
    } else {
        color_filter = board->black;
        enemy_color_filter = board->white;

        M_PAWN = M_PAWN_BLACK;
        M_PAWN_DOUBLE = M_PAWN_BLACK_DOUBLE;
        M_PAWN_TAKE = M_PAWN_BLACK_TAKE;

        canCastleShort = game->whiteCanCastleShort;
        canCastleLong = game->whiteCanCastleLong;
    }

    // Generate moves for all pieces

    // King
    List<Position> kingPositions = board_value_positions(board->king & color_filter);
    if (kingPositions.length() != 1) {
        // Dafuq?
        // return moves;  // Abort move generation, because no/many king?
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
            possibleMove.origin = shift_from_position(kingPosition);
            possibleMove.target = shift_from_position(moveTarget);
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, false)) {
                moves.add(possibleMove);
            }
        }

        // Castling
        if (canCastleShort && is_castle_legal(board, kingPosition, color, true)) {
            t_move possibleMove;
            possibleMove.origin = shift_from_position(kingPosition);
            possibleMove.target = shift_from_position(kingPosition + Position(2, 0));
            possibleMove.color = color;

            if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
                moves.add(possibleMove);
            }
        }
        if (canCastleLong && is_castle_legal(board, kingPosition, color, false)) {
            t_move possibleMove;
            possibleMove.origin = shift_from_position(kingPosition);
            possibleMove.target = shift_from_position(kingPosition - Position(2, 0));
            possibleMove.color = color;

            if (is_move_legal_nocheck(board, possibleMove, color_filter, enemy_color_filter, true)) {
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
            possibleMove.origin = shift_from_position(queenPosition);
            possibleMove.target = shift_from_position(moveTarget);
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
            possibleMove.origin = shift_from_position(rookPosition);
            possibleMove.target = shift_from_position(moveTarget);
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
            possibleMove.origin = shift_from_position(bishopPosition);
            possibleMove.target = shift_from_position(moveTarget);
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
            possibleMove.origin = shift_from_position(knightPosition);
            possibleMove.target = shift_from_position(moveTarget);
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
        if (targetOffset.isWithinBounds() && !(((pawnPosition.y == 6 && !color) || (pawnPosition.y == 1 && color)))) {
            // Should not be possible to not be within bounds
            int targetOffsetShift = shift_from_position(targetOffset.toPosition());
            bool isTargetOccupied = board_value_from_shift(color_filter | enemy_color_filter, targetOffsetShift);

            if (!isTargetOccupied) {
                possibleTargets.add(targetOffset.toPosition());
            }
        }

        // Double forward move
        if ((pawnPosition.y == 1 && !color) || (pawnPosition.y == 6 && color)) {
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
        if (!((pawnPosition.y == 6 && !color) || (pawnPosition.y == 1 && color))) {
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
        } else {
            for (int offsetIndex = 0; offsetIndex < M_PAWN_TAKE.length(); ++offsetIndex) {
                targetOffset = M_PAWN_TAKE.get(offsetIndex) + pawnPosition;

                if (targetOffset.isWithinBounds()) {
                    int targetOffsetShift = shift_from_position(targetOffset.toPosition());
                    bool isTargetOccupiedByEnemy = board_value_from_shift(enemy_color_filter, targetOffsetShift);

                    if (isTargetOccupiedByEnemy) {
                        t_move possibleMove;
                        possibleMove.origin = shift_from_position(pawnPosition);
                        possibleMove.target = targetOffsetShift;
                        possibleMove.color = color;
                        possibleMove.promoted = true;

                        if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, false)) {
                            for (int i = 0; i < 4; ++i) {
                                // Add one move for each promotable piece
                                possibleMove.promoted_to = i;

                                moves.add(possibleMove);
                            }
                        }
                    }
                }
            }
        }

        // En-passant
        if (game->enpassants != 0 && ((pawnPosition.y == 4 && !color) || (pawnPosition.y == 3 && color))) {
            // Only possible on the middle row of the opposing side
            int opposingPawnX = (int ) log2(game->enpassants);

            if (pawnPosition.x == opposingPawnX - 1 || pawnPosition.x == opposingPawnX + 1) {
                Position targetPosition = Position(opposingPawnX, (!color * 5) + (color * 2));

                int targetOffsetShift = shift_from_position(targetPosition);
                bool isTargetOccupiedByEnemy = board_value_from_shift(enemy_color_filter, targetOffsetShift);

                if (!isTargetOccupiedByEnemy) {
                    possibleTargets.add(targetPosition);
                }
            }
        }

        // Filter out illegal moves
        for (int targetIndex = 0; targetIndex < possibleTargets.length(); ++targetIndex) {
            Position moveTarget = possibleTargets.get(targetIndex);

            t_move possibleMove;
            possibleMove.origin = shift_from_position(pawnPosition);
            possibleMove.target = shift_from_position(moveTarget);
            possibleMove.color = color;

            if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, true)) {
                moves.add(possibleMove);
            }
        }

        // Promotion
        if ((pawnPosition.y == 6 && !color) || (pawnPosition.y == 1 && color)) {
            // Only possible on second to last row in the pawn's moving direction
            targetOffset = M_PAWN.get(0) + pawnPosition;
            if (targetOffset.isWithinBounds()) {
                // Should not be possible to not be within bounds
                int targetOffsetShift = shift_from_position(targetOffset.toPosition());
                bool isTargetOccupied = board_value_from_shift(color_filter | enemy_color_filter, targetOffsetShift);

                if (!isTargetOccupied) {
                    t_move possibleMove;
                    possibleMove.origin = shift_from_position(pawnPosition);
                    possibleMove.target = targetOffsetShift;
                    possibleMove.color = color;
                    possibleMove.promoted = true;

                    if (is_move_legal(board, possibleMove, color_filter, enemy_color_filter, false)) {
                        for (int i = 0; i < 4; ++i) {
                            // Add one move for each promotable piece
                            possibleMove.promoted_to = i;

                            moves.add(possibleMove);
                        }
                    }
                }
            }
        }
    }

    return moves;
}

bool is_enpassant(t_board *board, t_move *move) {
    if (!board_value_from_shift(board->pawn, move->origin)) {
        return false;
    }

    int shift_diff = abs((int )(move->target) - (int )(move->origin));
    if (!board_value_from_shift(board->white | board->black, move->target) && (shift_diff == 7 || shift_diff == 9)) {
        return true;
    }

    return false;
}

bool is_double_pawn_move(t_move *move) {
    if (abs((int )(move->target) - (int )(move->origin)) == 16) {
        return true;
    }
    return false;
}

bool is_castle(t_board *board, t_move *move) {
    int shift_diff = abs((int )(move->target) - (int )(move->origin));
    if (shift_diff == 2 && board_value_from_shift(board->king, move->origin)) {
        return true;
    }
    return false;
}

void doMove(t_board *board, t_move *move) {
    bool isEnpassant = is_enpassant(board, move);
    bool isCastle = is_castle(board, move);

    //generate bitmask for fields
    uint64_t bitOrigin = (uint64_t) 1 << move->origin;
    uint64_t bitTarget = (uint64_t) 1 << move->target;

    //set taken figure
    if ((board->queen & bitTarget) != 0) move->taken_figure = 1;
    else if ((board->rook & bitTarget) != 0) move->taken_figure = 2;
    else if ((board->bishop & bitTarget) != 0) move->taken_figure = 3;
    else if ((board->knight & bitTarget) != 0) move->taken_figure = 4;
    else if ((board->pawn & bitTarget) != 0) move->taken_figure = 5;
    else move->taken_figure = 0;

    //clear targetfield
    board->king &= ~bitTarget;
    board->queen &= ~bitTarget;
    board->rook &= ~bitTarget;
    board->bishop &= ~bitTarget;
    board->knight &= ~bitTarget;
    board->pawn &= ~bitTarget;
    board->white &= ~bitTarget;
    board->black &= ~bitTarget;

    //get originfigure and move it
    if ((board->king & bitOrigin) != 0) {
        board->king &= ~bitOrigin;
        board->king |= bitTarget;
    } else if ((board->queen & bitOrigin) != 0) {
        board->queen &= ~bitOrigin;
        board->queen |= bitTarget;
    } else if ((board->rook & bitOrigin) != 0) {
        board->rook &= ~bitOrigin;
        board->rook |= bitTarget;
    } else if ((board->bishop & bitOrigin) != 0) {
        board->bishop &= ~bitOrigin;
        board->bishop |= bitTarget;
    } else if ((board->knight & bitOrigin) != 0) {
        board->knight &= ~bitOrigin;
        board->knight |= bitTarget;
    } else if ((board->pawn & bitOrigin) != 0) {
        board->pawn &= ~bitOrigin;
        board->pawn |= bitTarget;
    }

    //clear originfield and set move color
    if ((board->black & bitOrigin) != 0) {
        board->black &= ~bitOrigin;
        move->color = 1;
    } else if ((board->white & bitOrigin) != 0) {
        board->white &= ~bitOrigin;
        move->color = 0;
    }

    //set color of targefield to move color
    if (move->color == 0) {
        board->black &= ~bitTarget;
        board->white |= bitTarget;
    } else if (move->color == 1) {
        board->white &= ~bitTarget;
        board->black |= bitTarget;
    }

    // Handle special moves
    if (isEnpassant) {
        Position originPosition = position_from_shift(move->origin);
        Position targetPosition = position_from_shift(move->target);
        Position takenPosition = Position(targetPosition.x, originPosition.y);

//        printf("EN-PASSANT: Removed piece at ");
//        printPosition(takenPosition);
//        printf("\n");

        int takenPositionShift = shift_from_position(takenPosition);
        uint64_t takenPositionBitmask = (uint64_t )1 << takenPositionShift;

        // Delete piece at selected position
        board->white &= ~takenPositionBitmask;
        board->black &= ~takenPositionBitmask;
        board->pawn &= ~takenPositionBitmask;
    }
    else if (isCastle) {
        t_move castleRookMove;
        if (move->origin < move->target) {
            // Short castle
            castleRookMove.origin = move->target + 1;
            castleRookMove.target = move->origin + 1;
            castleRookMove.color = move->color;
        } else {
            // Long castle
            castleRookMove.origin = move->target - 2;
            castleRookMove.target = move->origin - 1;
            castleRookMove.color = move->color;
        }
        doMove(board, &castleRookMove);
    }
    else if (move->promoted) {
        uint64_t targetPositionBitmask = (uint64_t )1 << move->target;
        // Remove pawn
        board->pawn &= ~targetPositionBitmask;

        // Add "new" piece
        if (move->promoted_to == 1) {
            board->queen |= targetPositionBitmask;
        } else if (move->promoted_to == 2) {
            board->rook |= targetPositionBitmask;
        } else if (move->promoted_to == 3) {
            board->bishop |= targetPositionBitmask;
        } else if (move->promoted_to == 4) {
            board->knight |= targetPositionBitmask;
        }
    }
}

void undoMove(t_board *board, t_move *move) {
    bool isEnpassant = is_enpassant(board, move);
    bool isCastle = is_castle(board, move);

    //generate bitmask for fields
    uint64_t bitOrigin = (uint64_t) 1 << move->origin;
    uint64_t bitTarget = (uint64_t) 1 << move->target;

    //get targetfigure and move it
    if ((board->king & bitTarget) != 0) {
        board->king &= ~bitTarget;
        board->king |= bitOrigin;
    } else if ((board->queen & bitTarget) != 0) {
        board->queen &= ~bitTarget;
        board->queen |= bitOrigin;
    } else if ((board->rook & bitTarget) != 0) {
        board->rook &= ~bitTarget;
        board->rook |= bitOrigin;
    } else if ((board->bishop & bitTarget) != 0) {
        board->bishop &= ~bitTarget;
        board->bishop |= bitOrigin;
    } else if ((board->knight & bitTarget) != 0) {
        board->knight &= ~bitTarget;
        board->knight |= bitOrigin;
    } else if ((board->pawn & bitTarget) != 0) {
        board->pawn &= ~bitTarget;
        board->pawn |= bitOrigin;
    }

    //clear targetfield
    board->black &= ~bitTarget;
    board->white &= ~bitTarget;

    //set color of originfield
    if (move->color == 0) {
        board->white |= bitOrigin;

        if (move->taken_figure) {
            board->black |= bitTarget;
        }
    } else if (move->color == 1) {
        board->black |= bitOrigin;

        if (move->taken_figure) {
            board->white |= bitTarget;
        }
    }

    //place takenfigure
    if (move->taken_figure == 0) {}
    else if (move->taken_figure == 1) board->queen |= bitTarget;
    else if (move->taken_figure == 2) board->rook |= bitTarget;
    else if (move->taken_figure == 3) board->bishop |= bitTarget;
    else if (move->taken_figure == 4) board->knight |= bitTarget;
    else if (move->taken_figure == 5) board->pawn |= bitTarget;

    if (isEnpassant) {
        Position originPosition = position_from_shift(move->origin);
        Position targetPosition = position_from_shift(move->target);
        Position takenPosition = Position(targetPosition.x, originPosition.y);

        int takenPositionShift = shift_from_position(takenPosition);
        uint64_t takenPositionBitmask = (uint64_t )1 << takenPositionShift;

        // Replace piece at selected position
        if (!move->color) {
            board->black |= takenPositionBitmask;
        } else {
            board->white |= takenPositionBitmask;
        }
        board->pawn |= takenPositionBitmask;
    }
    else if (isCastle) {
        t_move castleRookMove;
        if (move->origin < move->target) {
            // Short castle
            castleRookMove.origin = move->target + 1;
            castleRookMove.target = move->origin + 1;
            castleRookMove.color = move->color;
        } else {
            // Long castle
            castleRookMove.origin = move->target - 2;
            castleRookMove.target = move->origin - 1;
            castleRookMove.color = move->color;

            printf("Long castle move: ");
            printMove(castleRookMove);
        }
        undoMove(board, &castleRookMove);
    }
    else if (move->promoted) {
        uint64_t targetPositionBitmask = (uint64_t )1 << move->origin;
        // Reinsert pawn
        board->pawn |= targetPositionBitmask;

        // Remove "new" piece
        if (move->promoted_to == 1) {
            board->queen &= ~targetPositionBitmask;
        } else if (move->promoted_to == 2) {
            board->rook &= ~targetPositionBitmask;
        } else if (move->promoted_to == 3) {
            board->bishop &= ~targetPositionBitmask;
        } else if (move->promoted_to == 4) {
            board->knight &= ~targetPositionBitmask;
        }
    }
}

void printMove(const t_move move) {
    Position origin = position_from_shift(move.origin);
    Position target = position_from_shift(move.target);
    printf("Move %c%d -> %c%d\n", columnToLetter(origin.x), origin.y + 1, columnToLetter(target.x), target.y + 1);
}
