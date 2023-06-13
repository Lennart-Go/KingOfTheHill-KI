#include <cmath>
#include "move.h"
#include "board.h"
#include "game.h"
#include "end.h"
#include "moveMaps.h"


inline static constexpr uint8_t findFirst(const field pieces) {
    return static_cast<uint8_t>(log2(static_cast<double>(pieces & -pieces)));
}


inline static constexpr uint16_t occIdentifier(field moveMap, const field occ) {
    uint16_t occIdentifier = 0;
    for (long bitNumber = 0; moveMap != 0; bitNumber += bitNumber) {
        if ((occ & moveMap & -moveMap) != 0) {
            occIdentifier += bitNumber;
        }
        moveMap &= ((field )1 - moveMap);
    }

    return occIdentifier;
}

template<piece p> inline static constexpr field lookupSlider(const uint8_t piecePosition, const field occ) {
    switch (p) {
        case piece::rook: {
            field moveMap = rookMoves[piecePosition];
            uint16_t occId = occIdentifier(moveMap, occ);

            return rookLookup[64 * occId + piecePosition];
        }
        case piece::bishop: {
            field moveMap = bishopMoves[piecePosition];
            uint16_t occId = occIdentifier(moveMap, occ);

            return bishopLookup[64 * occId + piecePosition];
        }
        case piece::queen: {
            field rookMoveMap = rookMoves[piecePosition];
            field bishopMoveMap = bishopMoves[piecePosition];

            uint16_t rookOccId = occIdentifier(rookMoveMap, occ);
            uint16_t bishopOccId = occIdentifier(bishopMoveMap, occ);

            return rookLookup[64 * rookOccId + piecePosition] | bishopLookup[64 * bishopOccId + piecePosition];
        }
    }

    return (field )0;
}

template<piece p> inline static constexpr field pinLookUpSlider(const uint8_t piecePosition, const field occ) {
    switch (p) {
        case piece::rook: {
            field moveMap = rookMoves[piecePosition];
            uint16_t occId = occIdentifier(moveMap, occ);

            return rookPinLookup[64 * occId + piecePosition];
        }
        case piece::bishop: {
            field moveMap = bishopMoves[piecePosition];
            uint16_t occId = occIdentifier(moveMap, occ);

            return bishopPinLookup[64 * occId + piecePosition];
        }
        case piece::queen: {
            field rookMoveMap = rookMoves[piecePosition];
            field bishopMoveMap = bishopMoves[piecePosition];

            uint16_t rookOccId = occIdentifier(rookMoveMap, occ);
            uint16_t bishopOccId = occIdentifier(bishopMoveMap, occ);

            return rookPinLookup[64 * rookOccId + piecePosition] | bishopPinLookup[64 * bishopOccId + piecePosition];
        }
    }

    return (field )0;
}

template<piece p> inline static constexpr field lookup(const uint8_t piecePosition) {
    switch (p) {
        case piece::king: {
            return kingMoves[piecePosition];
        }
        case piece::knight: {
            return knightMoves[piecePosition];
        }
    }

    return (field )0;
}


template<bool color> std::vector<t_move> generate_moves(t_game *game) {
    t_board board = *game->board;
    field occ = board.occupied;

    uint8_t whiteKingShift = findFirst(board.whiteKing);
    uint8_t blackKingShift = findFirst(board.blackKing);

    if (color) {
        // Black's turn

        // --------------------------- //
        // Generate threatened squares //
        // --------------------------- //

        field threatened = 0;

        // Add fields covered by the king
        threatened |= lookup<piece::king>(whiteKingShift);

        // Add fields covered by the queens
        field queens = board.whiteQueen;
        field queensTargeting = 0;
        while (queens != 0) {
            queensTargeting |= lookupSlider<piece::queen>(findFirst(queens), occ);

            queens &= ((field )1 - queens);
        }
        threatened |= queensTargeting;
        queensTargeting |= board.whiteQueen;

        // Add fields covered by the rooks
        field rooks = board.whiteRook;
        field rooksTargeting = 0;
        while (rooks != 0) {
            rooksTargeting |= lookupSlider<piece::rook>(findFirst(rooks), occ);

            rooks &= ((field )1 - rooks);
        }
        threatened |= rooksTargeting;
        rooksTargeting |= board.whiteRook;

        // Add fields covered by the bishops
        field bishops = board.whiteBishop;
        field bishopsTargeting = 0;
        while (bishops != 0) {
            bishopsTargeting |= lookupSlider<piece::bishop>(findFirst(bishops), occ);
            bishops &= ((field )1 - bishops);
        }
        threatened |= bishopsTargeting;
        bishopsTargeting |= board.whiteBishop;

        // Add fields covered by the knights
        field knights = board.whiteKnight;
        while (knights != 0) {
            threatened |= lookup<piece::knight>(findFirst(knights));
            knights &= ((field )1 - knights);
        }

        // Add fields covered by pawns
        threatened |= (board.whitePawn & ~aFile) << 9;  // Taking to the left
        threatened |= (board.whitePawn & ~hFile) << 7;  // Taking to the right

        // Remove all fields covered by white pieces
        threatened &= ~board.white;


        // -------------------------------------------------------- //
        // Generate squares causing check and corresponding sliders //
        // -------------------------------------------------------- //

        field checks = 0;

        field blackKingMap = (field )1 << blackKingShift;
        if ((threatened & blackKingMap) != 0) {
            // There is at least one check

            // Check from queens
            checks |= lookupSlider<piece::queen>(blackKingShift, occ) & queensTargeting;

            // Check from rooks
            checks |= lookupSlider<piece::rook>(blackKingShift, occ) & rooksTargeting;

            // Check from bishops
            checks |= lookupSlider<piece::bishop>(blackKingShift, occ) & bishopsTargeting;

            // Check from knights
            checks |= lookup<piece::knight>(blackKingShift) & board.whiteKnight;

            // Check from pawns
            checks |= ((board.blackKing & ~aFile) >> 7) & board.whitePawn;  // Pawns threatening to the right
            checks |= ((board.blackKing & ~hFile) >> 9) & board.whitePawn;  // Pawns threatening to the left
        }

        // Handle different amounts of checking pieces
        field checkOrigins = checks & board.black;
        if (checkOrigins == 0) {
            // No checks -> Set all fields to 1
            checks = ~0;
        } else if ((checkOrigins & ((field )1 - checkOrigins)) != 0) {
            // More than one check -> Only King can move
            checks = 0;
        }

        int test = 0;


        // TODO: Generate pins and corresponding sliders

        // Friendly pieces
        // TODO: Generate king moves

        // TODO: Generate queen moves

        // TODO: Generate rook moves

        // TODO: Generate bishop moves

        // TODO: Generate knight moves

        // TODO: Generate pawn moves


        // Special moves
        // TODO: Generate pawn taking moves

        // TODO: Generate pawn pushing moves

        // TODO: Generate en-passant if possible

        // TODO: Generate promotions

        // TODO: Generate castles
    } else {
        // White's turn

        // TODO: Generate threatened squares

        // TODO: Generate squares causing check and corresponding sliders

        // TODO: Generate pins ans corresponding sliders

        // Friendly pieces
        // TODO: Generate king moves

        // TODO: Generate queen moves

        // TODO: Generate rook moves

        // TODO: Generate bishop moves

        // TODO: Generate knight moves

        // TODO: Generate pawn moves


        // Special moves
        // TODO: Generate pawn taking moves

        // TODO: Generate pawn pushing moves

        // TODO: Generate en-passant if possible

        // TODO: Generate promotions

        // TODO: Generate castles
    }

    return std::vector<t_move>();
}


bool is_enpassant(t_board *board, t_move *move) {
    if (!board_value_from_shift(board->pawn, move->origin)) {
        return false;
    }

    int shift_diff = abs((int) (move->target) - (int) (move->origin));
    if (!board_value_from_shift(board->white | board->black, move->target) && (shift_diff == 7 || shift_diff == 9)) {
        return true;
    }

    return false;
}

bool is_double_pawn_move(t_board *board, t_move *move) {
    if (!board_value_from_shift(board->pawn, move->origin)) {
        return false;
    }

    if (abs((int) (move->target) - (int) (move->origin)) == 16) {
        return true;
    }
    return false;
}

bool is_castle(t_board *board, t_move *move) {
    int shift_diff = abs((int) (move->target) - (int) (move->origin));
    if (shift_diff == 2 && board_value_from_shift(board->king, move->origin)) {
        return true;
    }
    return false;
}

void doMove(t_board *board, t_move *move) {
    bool isEnpassant = is_enpassant(board, move);
    bool isCastle = is_castle(board, move);

    //generate bitmask for fields
    field bitOrigin = (field) 1 << move->origin;
    field bitTarget = (field) 1 << move->target;

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

    // TODO: Idea: Instead of using if condition for each piece, calculate dynamically?
    // TODO:        board->piece |= board_value_from_shift(board->piece, move->origin) * bitTarget;
    // TODO:        board->piece &= board_value_from_shift(board->piece, move->origin) * ~bitOrigin;
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
        field takenPositionBitmask = (field) 1 << takenPositionShift;

        // Delete piece at selected position
        board->white &= ~takenPositionBitmask;
        board->black &= ~takenPositionBitmask;
        board->pawn &= ~takenPositionBitmask;
    } else if (isCastle) {
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
    } else if (move->promoted) {
        field targetPositionBitmask = (field) 1 << move->target;
        // Remove pawn
        board->pawn &= ~targetPositionBitmask;

        // Add "new" piece
        if (move->promoted_to == 0) {
            board->queen |= targetPositionBitmask;
        } else if (move->promoted_to == 1) {
            board->rook |= targetPositionBitmask;
        } else if (move->promoted_to == 2) {
            board->bishop |= targetPositionBitmask;
        } else if (move->promoted_to == 3) {
            board->knight |= targetPositionBitmask;
        }
    }
}

void undoMove(t_board *board, t_move *move) {
    //generate bitmask for fields
    field bitOrigin = (field) 1 << move->origin;
    field bitTarget = (field) 1 << move->target;

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

    bool isEnpassant = is_enpassant(board, move);
    bool isCastle = is_castle(board, move);

    if (isEnpassant) {
        Position originPosition = position_from_shift(move->origin);
        Position targetPosition = position_from_shift(move->target);
        Position takenPosition = Position(targetPosition.x, originPosition.y);

        int takenPositionShift = shift_from_position(takenPosition);
        field takenPositionBitmask = (field) 1 << takenPositionShift;

        // printf("Replacing ");
        // Replace piece at selected position
        if (!move->color) {
            board->black |= takenPositionBitmask;
            // printf("black ");
        } else {
            board->white |= takenPositionBitmask;
            // printf("white ");
        }
        board->pawn |= takenPositionBitmask;
        // printf("pawn to ");
        // printPosition(takenPosition);
        // printf("\n");
    } else if (isCastle) {
        t_move castleRookMove;
        if (move->origin < move->target) {
            // Short castle
            castleRookMove.origin = move->target + 1;
            castleRookMove.target = move->target - 1;
            castleRookMove.color = move->color;
        } else {
            // Long castle
            castleRookMove.origin = move->target - 2;
            castleRookMove.target = move->target + 1;
            castleRookMove.color = move->color;

//            printf("Long castle move: ");
//            printMove(castleRookMove);
        }
        undoMove(board, &castleRookMove);
    } else if (move->promoted) {
        field targetPositionBitmask = (field) 1 << move->origin;
        // Reinsert pawn
        board->pawn |= targetPositionBitmask;

        // Remove "new" piece
        if (move->promoted_to == 0) {
            board->queen &= ~targetPositionBitmask;
        } else if (move->promoted_to == 1) {
            board->rook &= ~targetPositionBitmask;
        } else if (move->promoted_to == 2) {
            board->bishop &= ~targetPositionBitmask;
        } else if (move->promoted_to == 3) {
            board->knight &= ~targetPositionBitmask;
        }
    }

    if (countFigure(board->pawn & board->white) > 8) {
        board->pawn = board->pawn;
    }
}

void commitMove(t_game *game, t_move *move) {
    if (move->origin == move->target) {
        // Failure move -> No moves generated
        game->isOver = true;
        return;
    }

    doMove(game->board, move);

    move->enpassants = game->enpassants;  // Save previous enpassant possibilities to enable reverting
    game->enpassants = 0;

    // Handle double-forward pawn move
    if (is_double_pawn_move(game->board, move)) {
        //printf("DOUBLE PAWN MOVE!\n");
        // Set according bit if the move was a double-forward pawn move
        game->enpassants = position_from_shift(move->origin).x + 1;
    } else if (is_castle(game->board, move)) {
        //printf("CASTLE!\n");
    } else if (is_enpassant(game->board, move)) {
        //printf("EN-PASSANT!\n");
    } else if (move->promoted) {
        //printf("PROMOTION!\n");
    }

//    winner_t gameEnd = checkEnd(game, game->turn);
//    if (gameEnd) {
//        game->isOver = true;
//
//        if (gameEnd == WHITE) {
//            game->whiteWon = true;
//        } else if (gameEnd == BLACK) {
//            game->blackWon = true;
//        }
//
//        return;
//    }

    game->turn = !game->turn;

    // TODO: Update moveTime
    if (!move->color) {
        // White moved

        // Update castling flags
        move->disable_short_castle &= game->whiteCanCastleShort;
        move->disable_long_castle &= game->whiteCanCastleLong;

        if (!game->whiteCastled && (game->whiteCanCastleShort || game->whiteCanCastleLong)) {
            if (move->castled_short || move->castled_long) {
                game->whiteCastled = true;
            }

            game->whiteCanCastleShort ^= move->disable_short_castle;
            game->whiteCanCastleLong ^= move->disable_long_castle;
        } else {
            move->disable_short_castle = false;
            move->disable_long_castle = false;
        }

        game->whiteMoveCounter++;
    } else {
        // Black moved

        // Update castling flags
        move->disable_short_castle &= game->blackCanCastleShort;
        move->disable_long_castle &= game->blackCanCastleLong;

        if (!game->blackCastled && (game->blackCanCastleShort || game->blackCanCastleLong)) {
            if (move->castled_short || move->castled_long) {
                game->blackCastled = true;
            }

            game->blackCanCastleShort ^= move->disable_short_castle;
            game->blackCanCastleLong ^= move->disable_long_castle;
        }


        game->blackMoveCounter++;
    }
}

void revertMove(t_game *game, t_move *move) {
    if (move->origin == move->target) {
        // Failure move -> No moves were possible
        game->isOver = false;
        return;
    }

    game->enpassants = move->enpassants;

    if (game->isOver) {
        game->isOver = false;
        game->whiteWon = false;
        game->blackWon = false;

        undoMove(game->board, move);
        return;
    }

    game->turn = !game->turn;

    if (!move->color) {
        // White moved

        // Update castling flags
        if (game->whiteCastled || !(game->whiteCanCastleShort && game->whiteCanCastleLong)) {
            if (move->castled_short || move->castled_long) {
                game->whiteCastled = false;
            }
            game->whiteCanCastleShort ^= move->disable_short_castle;
            game->whiteCanCastleLong ^= move->disable_long_castle;
        }

        game->whiteMoveCounter--;
    } else {
        // Black moved

        // Update castling flags
        if (game->blackCastled || !(game->blackCanCastleShort && game->blackCanCastleLong)) {
            if (move->castled_short || move->castled_long) {
                game->blackCastled = false;
            }
            game->blackCanCastleShort ^= move->disable_short_castle;
            game->blackCanCastleLong ^= move->disable_long_castle;
        }

        game->blackMoveCounter--;
    }

    // positionTrackingUndo(game);  // TODO: Reinsert
    undoMove(game->board, move);
}

void printMove(const t_move move) {
    Position origin = position_from_shift(move.origin);
    Position target = position_from_shift(move.target);
    printf("Move %c%d -> %c%d", columnToLetter(origin.x), origin.y + 1, columnToLetter(target.x), target.y + 1);
}
