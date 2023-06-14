#include "util.h"
#include "board.h"
#include "game.h"
#include "moveMaps.h"
#include <cmath>

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
    unsigned taken_figure:3 = 0;  // 0 -> Non taken, 1 -> Queen, 2 -> Rook, 3 -> Bishop, 4 -> Knight, 5 -> Pawn (?)
    unsigned color:1;
    unsigned promoted:1 = 0;
    unsigned promoted_to:2 = 0;
    unsigned castled_short:1 = 0;
    unsigned castled_long:1 = 0;
    unsigned enpassants:4 = 0;
    unsigned disable_short_castle:1 = 0;
    unsigned disable_long_castle:1 = 0;
} t_move;

#define O_MOVE_ORIGIN 0
#define O_MOVE_TARGET 6
#define O_MOVE_TAKEN_FIGURE 12
#define O_MOVE_COLOR 15


bool empty_between(t_board *board, t_move move);
bool is_move_check(t_board *board, t_move move);
bool is_threatened(t_board *board, Position target, bool color);
bool is_move_legal(t_board *board, t_move move, field color_filter, field enemy_color_filter, bool checkBetween);
bool is_move_legal_nocheck(t_board *board, t_move move, field color_filter, field enemy_color_filter, bool checkBetween);
bool is_castle_legal(t_board *board, Position kingPosition, bool color, bool direction);

void printMove(t_move move);

bool is_enpassant(t_board *board, t_move *move);
bool is_double_pawn_move(t_board *board, t_move *move);
bool is_castle(t_board *board, t_move *move);

void doMove(t_board *board, t_move* move);
void undoMove(t_board *board, t_move* move);

void commitMove(t_game *game, t_move *move);
void revertMove(t_game *game, t_move *move);


inline static constexpr uint8_t findFirst(const field pieces) {
    return static_cast<uint8_t>(log2(static_cast<double>(pieces & -pieces)));
}


inline static constexpr uint16_t occIdentifier(field moveMap, const field occ) {
    uint16_t occIdentifier = 0;
    for (long bitNumber = 1; moveMap != 0; bitNumber += bitNumber) {
        if ((occ & moveMap & -moveMap) != 0) {
            occIdentifier += bitNumber;
        }
        moveMap &= (moveMap - 1);
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

            queens &= (queens - 1);
        }
        threatened |= queensTargeting;
        queensTargeting |= board.whiteQueen;

        // Add fields covered by the rooks
        field rooks = board.whiteRook;
        field rooksTargeting = 0;
        while (rooks != 0) {
            rooksTargeting |= lookupSlider<piece::rook>(findFirst(rooks), occ);

            rooks &= (rooks - 1);
        }
        threatened |= rooksTargeting;
        rooksTargeting |= board.whiteRook;

        // Add fields covered by the bishops
        field bishops = board.whiteBishop;
        field bishopsTargeting = 0;
        while (bishops != 0) {
            bishopsTargeting |= lookupSlider<piece::bishop>(findFirst(bishops), occ);
            bishops &= (bishops - 1);
        }
        threatened |= bishopsTargeting;
        bishopsTargeting |= board.whiteBishop;

        // Add fields covered by the knights
        field knights = board.whiteKnight;
        while (knights != 0) {
            threatened |= lookup<piece::knight>(findFirst(knights));
            knights &= (knights - 1);
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

            field kingRookLookup = lookupSlider<piece::rook>(blackKingShift, occ);
            field kingBishopLookup = lookupSlider<piece::rook>(blackKingShift, occ);
            field kingKnightLookup = lookupSlider<piece::rook>(blackKingShift, occ);

            // Check from queens
            // TODO: Only check one "direction" at a time (up+down, left+right, upleft+downright, upright+downleft)
            checks |= kingRookLookup & queensTargeting;
            checks |= kingBishopLookup & queensTargeting;

            // Check from rooks
            checks |= kingRookLookup & rooksTargeting;

            // Check from bishops
            checks |= kingBishopLookup & bishopsTargeting;

            // Check from knights
            checks |= kingKnightLookup & board.whiteKnight;

            // Check from pawns
            checks |= ((board.blackKing & ~aFile) >> 7) & board.whitePawn;  // Pawns threatening to the right
            checks |= ((board.blackKing & ~hFile) >> 9) & board.whitePawn;  // Pawns threatening to the left
        }

        // Handle different amounts of checking pieces
        field checkOrigins = checks & board.white;
        if (checkOrigins == 0) {
            // No checks -> Set all fields to 1
            checks = ~0;
        } else if ((checkOrigins & (checkOrigins - 1)) != 0) {
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

    return {};
}



#endif //KINGOFTHEHILL_KI_MOVE_H
