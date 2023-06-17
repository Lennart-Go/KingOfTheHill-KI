#include <cmath>

#include "util.h"
#include "board.h"
#include "moveMaps.h"

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

typedef struct move_old {
    unsigned origin: 6;
    unsigned target: 6;
    unsigned taken_figure: 3 = 0;  // 0 -> Non taken, 1 -> Queen, 2 -> Rook, 3 -> Bishop, 4 -> Knight, 5 -> Pawn (?)
    unsigned color: 1;
    unsigned promoted: 1 = 0;
    unsigned promoted_to: 2 = 0;
    unsigned castled_short: 1 = 0;
    unsigned castled_long: 1 = 0;
    unsigned enpassants: 4 = 0;
    unsigned disable_short_castle: 1 = 0;
    unsigned disable_long_castle: 1 = 0;

    move_old(short org, short trg, bool col) : origin(org), target(trg), color(col) {}
    move_old() : origin(0), target(0), color(0) {}
} t_move_old;


typedef struct move {
    const field originMap;
    const field targetMap;

    move() : originMap(0), targetMap(0) {}
    move(field origin, field target) : originMap(origin), targetMap(target) {}
    move(const move &other) = default;
} t_move;


typedef struct gameState {
    const t_board board;
    const t_move move;

    const unsigned wCastleShort: 1;
    const unsigned wCastleLong: 1;
    const unsigned bCastleShort: 1;
    const unsigned bCastleLong: 1;

    const unsigned enpassant: 4;

    gameState(const t_board &brd, t_move mov,
                        unsigned whiteCastleShort, unsigned whiteCastleLong,
                        unsigned blackCastleShort, unsigned blackCastleLong,
                        short en_passant) :
            board(brd), move(mov),
            wCastleShort(whiteCastleShort), wCastleLong(whiteCastleLong),
            bCastleShort(blackCastleShort), bCastleLong(blackCastleLong),
            enpassant(en_passant) {}
    gameState(const t_board &brd, t_move mov) :
            board(brd), move(mov),
            wCastleShort(true), wCastleLong(true),
            bCastleShort(true), bCastleLong(true),
            enpassant(0) {}

    gameState(const gameState &state) = default;
} t_gameState;


inline static uint8_t findFirst(const field pieces) {
    return static_cast<uint8_t>(log2(static_cast<double>(pieces & -pieces)));
}


inline static uint16_t occIdentifier(field moveMap, const field occ) {
    uint16_t occIdentifier = 0;
    for (long bitNumber = 1; moveMap != 0; bitNumber += bitNumber) {
        if ((occ & moveMap & -moveMap) != 0) {
            occIdentifier += bitNumber;
        }
        moveMap &= (moveMap - 1);
    }

    return occIdentifier;
}


template<piece p, bool color>
inline static t_board move(t_board currentBoard, field origin, field target) {
    field wk = currentBoard.whiteKing;
    field wq = currentBoard.whiteQueen;
    field wr = currentBoard.whiteRook;
    field wb = currentBoard.whiteBishop;
    field wn = currentBoard.whiteKnight;
    field wp = currentBoard.whitePawn;

    field bk = currentBoard.blackKing;
    field bq = currentBoard.blackQueen;
    field br = currentBoard.blackRook;
    field bb = currentBoard.blackBishop;
    field bn = currentBoard.blackKnight;
    field bp = currentBoard.blackPawn;

    field move = origin | target;

    if (color) {
        // Black is moving

        if (p == piece::king)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk ^ move, bq, br, bb, bn, bp};
        if (p == piece::queen)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk, bq ^ move, br, bb, bn, bp};
        if (p == piece::rook)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk, bq, br ^ move, bb, bn, bp};
        if (p == piece::bishop)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk, bq, br, bb ^ move, bn, bp};
        if (p == piece::knight)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk, bq, br, bb, bn ^ move, bp};
        if (p == piece::pawn)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk, bq, br, bb, bn, bp ^ move};
        if (p == piece::none)
            return {wk & ~target, wq & ~target, wr & ~target, wb & ~target, wn & ~target, wp & ~target,
                    bk, bq, br, bb, bn, bp};
    } else {
        // White is moving

        if (p == piece::king)
            return {wk ^ move, wq, wr, wb, wn, wp,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
        if (p == piece::queen)
            return {wk, wq ^ move, wr, wb, wn, wp,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
        if (p == piece::rook)
            return {wk, wq, wr ^ move, wb, wn, wp,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
        if (p == piece::bishop)
            return {wk, wq, wr, wb ^ move, wn, wp,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
        if (p == piece::knight)
            return {wk, wq, wr, wb, wn ^ move, wp,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
        if (p == piece::pawn)
            return {wk, wq, wr, wb, wn, wp ^ move,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
        if (p == piece::none)
            return {wk, wq, wr, wb, wn, wp,
                    bk & ~target, bq & ~target, br & ~target, bb & ~target, bn & ~target, bp & ~target};
    }
}


template <piece p, bool color>
inline static t_board placePiece(t_board currentBoard, field target) {
    field wk = currentBoard.whiteKing;
    field wq = currentBoard.whiteQueen;
    field wr = currentBoard.whiteRook;
    field wb = currentBoard.whiteBishop;
    field wn = currentBoard.whiteKnight;
    field wp = currentBoard.whitePawn;

    field bk = currentBoard.blackKing;
    field bq = currentBoard.blackQueen;
    field br = currentBoard.blackRook;
    field bb = currentBoard.blackBishop;
    field bn = currentBoard.blackKnight;
    field bp = currentBoard.blackPawn;

    if (color) {
        // Black is moving

        if (p == piece::king)
            return {wk, wq, wr, wb, wn, wp,
                    bk | target, bq, br, bb, bn, bp};
        if (p == piece::queen)
            return {wk, wq, wr, wb, wn, wp,
                    bk, bq | target, br, bb, bn, bp};
        if (p == piece::rook)
            return {wk, wq, wr, wb, wn, wp,
                    bk, bq, br | target, bb, bn, bp};
        if (p == piece::bishop)
            return {wk, wq, wr, wb, wn, wp,
                    bk, bq, br, bb | target, bn, bp};
        if (p == piece::knight)
            return {wk, wq, wr, wb, wn, wp,
                    bk, bq, br, bb, bn | target, bp};
        if (p == piece::pawn)
            return {wk, wq, wr, wb, wn, wp,
                    bk, bq, br, bb, bn, bp | target};
        if (p == piece::none)
            return {wk, wq, wr, wb, wn, wp,
                    bk, bq, br, bb, bn, bp};
    } else {
        // White is moving

        if (p == piece::king)
            return {wk | target, wq, wr, wb, wk, wp,
                    bk, bq, br, bb, bn, bp};
        if (p == piece::queen)
            return {wk, wq | target, wr, wb, wk, wp,
                    bk, bq, br, bb, bn, bp};
        if (p == piece::rook)
            return {wk, wq, wr | target, wb, wk, wp,
                    bk, bq, br, bb, bn, bp};
        if (p == piece::bishop)
            return {wk, wq, wr, wb | target, wk, wp,
                    bk, bq, br, bb, bn, bp};
        if (p == piece::knight)
            return {wk, wq, wr, wb, wk | target, wp,
                    bk, bq, br, bb, bn, bp};
        if (p == piece::pawn)
            return {wk, wq, wr, wb, wk, wp | target,
                    bk, bq, br, bb, bn, bp};
        if (p == piece::none)
            return {wk, wq, wr, wb, wk, wp,
                    bk, bq, br, bb, bn, bp};
    }
}


template<bool color>
inline static void moveKing(std::vector<t_gameState> *moves, t_gameState currentState, field origin, field targets) {
    unsigned wCastleShort, wCastleLong, bCastleShort, bCastleLong;
    if (color) {
        wCastleShort = currentState.wCastleShort;
        wCastleLong = currentState.wCastleLong;

        bCastleShort = false;
        bCastleLong = false;
    } else {
        wCastleShort = false;
        wCastleLong = false;

        bCastleShort = currentState.bCastleShort;
        bCastleLong = currentState.bCastleLong;
    }

    field currentTarget;
    while (targets != 0) {
        currentTarget = (targets & -targets);

        t_board targetBoard = move<piece::king, color>(currentState.board, origin, currentTarget);
        t_move mov = t_move(origin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                wCastleShort, wCastleLong,
                                bCastleShort, bCastleLong,
                                0));

        targets &= (targets - 1);
    }
}


template<bool color>
inline static void moveKingCastleShort(std::vector<t_gameState> *moves, t_gameState currentState) {
    unsigned wCastleShort, wCastleLong, bCastleShort, bCastleLong;
    if (color) {
        wCastleShort = currentState.wCastleShort;
        wCastleLong = currentState.wCastleLong;

        bCastleShort = false;
        bCastleLong = false;

        field kingOrigin = currentState.board.blackKing;
        field kingTarget = kingOrigin << 2;

        field rookOrigin = 0b00000000000000000000000000000000000000000000000010000000;
        field rookTarget = 0b00000000000000000000000000000000000000000000000000100000;

        t_board targetBoardKingMoved = move<piece::king, color>(currentState.board, kingOrigin, kingTarget);
        t_board targetBoard = move<piece::rook, color>(targetBoardKingMoved, rookOrigin, rookTarget);
        t_move mov = t_move(kingOrigin, kingTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   wCastleShort, wCastleLong,
                                   bCastleShort, bCastleLong,
                                   0));
    } else {
        wCastleShort = false;
        wCastleLong = false;

        bCastleShort = currentState.bCastleShort;
        bCastleLong = currentState.bCastleLong;

        field kingOrigin = currentState.board.blackKing;
        field kingTarget = kingOrigin << 2;

        field rookOrigin = 0b10000000000000000000000000000000000000000000000000000000;
        field rookTarget = 0b00100000000000000000000000000000000000000000000000000000;

        t_board targetBoardKingMoved = move<piece::king, color>(currentState.board, kingOrigin, kingTarget);
        t_board targetBoard = move<piece::rook, color>(targetBoardKingMoved, rookOrigin, rookTarget);
        t_move mov = t_move(kingOrigin, kingTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   wCastleShort, wCastleLong,
                                   bCastleShort, bCastleLong,
                                   0));
    }
}


template<bool color>
inline static void moveKingCastleLong(std::vector<t_gameState> *moves, t_gameState currentState) {
    unsigned wCastleShort, wCastleLong, bCastleShort, bCastleLong;
    if (color) {
        wCastleShort = currentState.wCastleShort;
        wCastleLong = currentState.wCastleLong;

        bCastleShort = false;
        bCastleLong = false;

        field kingOrigin = currentState.board.blackKing;
        field kingTarget = kingOrigin >> 2;

        field rookOrigin = 0b00000000000000000000000000000000000000000000000000000001;
        field rookTarget = 0b00000000000000000000000000000000000000000000000000001000;

        t_board targetBoardKingMoved = move<piece::king, color>(currentState.board, kingOrigin, kingTarget);
        t_board targetBoard = move<piece::rook, color>(targetBoardKingMoved, rookOrigin, rookTarget);
        t_move mov = t_move(kingOrigin, kingTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   wCastleShort, wCastleLong,
                                   bCastleShort, bCastleLong,
                                   0));
    } else {
        wCastleShort = false;
        wCastleLong = false;

        bCastleShort = currentState.bCastleShort;
        bCastleLong = currentState.bCastleLong;

        field kingOrigin = currentState.board.blackKing;
        field kingTarget = kingOrigin >> 2;

        field rookOrigin = 0b00000001000000000000000000000000000000000000000000000000;
        field rookTarget = 0b00001000000000000000000000000000000000000000000000000000;

        t_board targetBoardKingMoved = move<piece::king, color>(currentState.board, kingOrigin, kingTarget);
        t_board targetBoard = move<piece::rook, color>(targetBoardKingMoved, rookOrigin, rookTarget);
        t_move mov = t_move(kingOrigin, kingTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   wCastleShort, wCastleLong,
                                   bCastleShort, bCastleLong,
                                   0));
    }
}


template<bool color>
inline static void moveQueens(std::vector<t_gameState> *moves, t_gameState currentState, uint8_t originShift, field targets) {
    field origin = (field )1 << originShift;

    field currentTarget;
    while (targets != 0) {
        currentTarget = (targets & -targets);

        t_board targetBoard = move<piece::queen, color>(currentState.board, origin, currentTarget);
        t_move mov = t_move(origin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   currentState.wCastleShort, currentState.wCastleLong,
                                   currentState.bCastleShort, currentState.bCastleLong,
                                0));

        targets &= (targets - 1);
    }
}


template<bool color>
inline static void moveRooks(std::vector<t_gameState> *moves, t_gameState currentState, uint8_t originShift, field targets) {
    field origin = (field )1 << originShift;

    unsigned wCastleShort, wCastleLong, bCastleShort, bCastleLong;
    if (color) {
        wCastleShort = currentState.wCastleShort;
        wCastleLong = currentState.wCastleLong;

        bCastleShort = originShift != 7;
        bCastleLong = originShift != 0;
    } else {
        wCastleShort = originShift != 63;
        wCastleLong = originShift != 56;

        bCastleShort = currentState.bCastleShort;
        bCastleLong = currentState.bCastleLong;
    }

    field currentTarget;
    while (targets != 0) {
        currentTarget = (targets & -targets);

        t_board targetBoard = move<piece::rook, color>(currentState.board, origin, currentTarget);
        t_move mov = t_move(origin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                wCastleShort, wCastleLong,
                                bCastleShort, bCastleLong,
                                0));

        targets &= (targets - 1);
    }
}


template<bool color>
inline static void moveBishops(std::vector<t_gameState> *moves, t_gameState currentState, uint8_t originShift, field targets) {
    field origin = (field )1 << originShift;

    field currentTarget;
    while (targets != 0) {
        currentTarget = (targets & -targets);

        t_board targetBoard = move<piece::bishop, color>(currentState.board, origin, currentTarget);
        t_move mov = t_move(origin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   currentState.wCastleShort, currentState.wCastleLong,
                                   currentState.bCastleShort, currentState.bCastleLong,
                                0));

        targets &= (targets - 1);
    }
}


template<bool color>
inline static void moveKnights(std::vector<t_gameState> *moves, t_gameState currentState, uint8_t originShift, field targets) {
    field origin = (field )1 << originShift;

    field currentTarget;
    while (targets != 0) {
        currentTarget = (targets & -targets);

        t_board targetBoard = move<piece::knight, color>(currentState.board, origin, currentTarget);
        t_move mov = t_move(origin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                   currentState.wCastleShort, currentState.wCastleLong,
                                   currentState.bCastleShort, currentState.bCastleLong,
                                0));

        targets &= (targets - 1);
    }
}


template<bool color>
inline static void movePawns(std::vector<t_gameState> *moves, t_gameState currentState, field origins, field targets) {
    // TODO: Handle promotions

    field currentOrigin, currentTarget;
    while (targets != 0) {
        currentOrigin = (origins & -origins);
        currentTarget = (targets & -targets);

        t_board targetBoard = move<piece::pawn, color>(currentState.board, currentOrigin, currentTarget);
        t_move mov = t_move(currentOrigin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                currentState.wCastleShort, currentState.wCastleLong,
                                currentState.bCastleShort, currentState.bCastleLong,
                                0));

        origins &= (origins - 1);
        targets &= (targets - 1);
    }
}


template<bool color>
inline static void movePawnsPromotion(std::vector<t_gameState> *moves, t_gameState currentState, field origins, field targets) {
    field currentOrigin, currentTarget;
    while (targets != 0) {
        currentOrigin = (origins & -origins);
        currentTarget = (targets & -targets);

        t_board targetBoardPrePromotion = move<piece::pawn, color>(currentState.board, currentOrigin, currentTarget);

        {
            // Promote to queen
            t_board targetBoardPieceRemoved = move<piece::none, color>(targetBoardPrePromotion, 0, currentTarget);
            t_board targetBoard = placePiece<piece::queen, color>(targetBoardPieceRemoved, currentTarget);

            t_move mov = t_move(currentOrigin, currentTarget);
            moves->push_back(gameState(targetBoard, mov,
                                       currentState.wCastleShort, currentState.wCastleLong,
                                       currentState.bCastleShort, currentState.bCastleLong,
                                       0));
        }
        {
            // Promote to rook
            t_board targetBoardPieceRemoved = move<piece::none, color>(targetBoardPrePromotion, 0, currentTarget);
            t_board targetBoard = placePiece<piece::rook, color>(targetBoardPieceRemoved, currentTarget);

            t_move mov = t_move(currentOrigin, currentTarget);
            moves->push_back(gameState(targetBoard, mov,
                                       currentState.wCastleShort, currentState.wCastleLong,
                                       currentState.bCastleShort, currentState.bCastleLong,
                                       0));
        }
        {
            // Promote to bishop
            t_board targetBoardPieceRemoved = move<piece::none, color>(targetBoardPrePromotion, 0, currentTarget);
            t_board targetBoard = placePiece<piece::bishop, color>(targetBoardPieceRemoved, currentTarget);

            t_move mov = t_move(currentOrigin, currentTarget);
            moves->push_back(gameState(targetBoard, mov,
                                       currentState.wCastleShort, currentState.wCastleLong,
                                       currentState.bCastleShort, currentState.bCastleLong,
                                       0));
        }
        {
            // Promote to knight
            t_board targetBoardPieceRemoved = move<piece::none, color>(targetBoardPrePromotion, 0, currentTarget);
            t_board targetBoard = placePiece<piece::knight, color>(targetBoardPieceRemoved, currentTarget);

            t_move mov = t_move(currentOrigin, currentTarget);
            moves->push_back(gameState(targetBoard, mov,
                                       currentState.wCastleShort, currentState.wCastleLong,
                                       currentState.bCastleShort, currentState.bCastleLong,
                                       0));
        }

        origins &= (origins - 1);
        targets &= (targets - 1);
    }
}


template <bool color>
inline static void movePawnsPush(std::vector<t_gameState> *moves, t_gameState currentState, field origins, field targets) {
    uint8_t currentFile;
    field currentOrigin, currentTarget;
    while (targets != 0) {
        currentOrigin = (origins & -origins);
        currentTarget = (targets & -targets);

        currentFile = findFirst(currentOrigin) % 8;

        t_board targetBoard = move<piece::pawn, color>(currentState.board, currentOrigin, currentTarget);
        t_move mov = t_move(currentOrigin, currentTarget);
        moves->push_back(gameState(targetBoard, mov,
                                currentState.wCastleShort, currentState.wCastleLong,
                                currentState.bCastleShort, currentState.bCastleLong,
                                currentFile));

        origins &= (origins - 1);
        targets &= (targets - 1);
    }
}


template <bool color>
inline static void movePawnsEnPassant(std::vector<t_gameState> *moves, t_gameState currentState, field origins, field target) {
    field targetPawn;
    if (color) {
        targetPawn = target >> 8;
    } else {
        targetPawn = target << 8;
    }

    field currentOrigin;
    while (origins != 0) {
        currentOrigin = (origins & -origins);

        t_board targetBoardPreTake = move<piece::pawn, color>(currentState.board, currentOrigin, target);
        t_board targetBoard = move<piece::none, color>(targetBoardPreTake, 0, targetPawn);
        t_move mov = t_move(currentOrigin, target);
        moves->push_back(gameState(targetBoard, mov,
                                currentState.wCastleShort, currentState.wCastleLong,
                                currentState.bCastleShort, currentState.wCastleLong,
                                0));

        origins &= (origins - 1);
    }
}


// TODO: Move function for castles






template<piece p>
inline static field lookupSlider(const uint8_t piecePosition, const field occ) {
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

    return (field) 0;
}

template<piece p>
inline static field lookupPinSlider(const uint8_t piecePosition, const field occ) {
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

    return (field) 0;
}

template<piece p>
inline static field lookup(const uint8_t piecePosition) {
    switch (p) {
        case piece::king: {
            return kingMoves[piecePosition];
        }
        case piece::knight: {
            return knightMoves[piecePosition];
        }
    }

    return (field) 0;
}


template<bool color>
inline field getThreatened(const t_board board) {
    field occ = board.occupied;
    field threatened = 0;

    if (color) {
        // Add fields covered by the king
        threatened |= lookup<piece::king>(board.whiteKing);

        // Add fields covered by the queens
        field queens = board.whiteQueen;
        while (queens != 0) {
            threatened |= lookupSlider<piece::queen>(findFirst(queens), occ);

            queens &= (queens - 1);
        }

        // Add fields covered by the rooks
        field rooks = board.whiteRook;
        while (rooks != 0) {
            threatened |= lookupSlider<piece::rook>(findFirst(rooks), occ);

            rooks &= (rooks - 1);
        }

        // Add fields covered by the bishops
        field bishops = board.whiteBishop;
        while (bishops != 0) {
            threatened |= lookupSlider<piece::bishop>(findFirst(bishops), occ);
            bishops &= (bishops - 1);
        }

        // Add fields covered by the knights
        field knights = board.whiteKnight;
        while (knights != 0) {
            threatened |= lookup<piece::knight>(findFirst(knights));
            knights &= (knights - 1);
        }

        // Add fields covered by pawns
        threatened |= (board.whitePawn & ~aFile) >> 9;  // Taking to the left
        threatened |= (board.whitePawn & ~hFile) >> 7;  // Taking to the right
    } else {
        // Add fields covered by the king
        threatened |= lookup<piece::king>(board.blackKing);

        // Add fields covered by the queens
        field queens = board.blackQueen;
        while (queens != 0) {
            threatened |= lookupSlider<piece::queen>(findFirst(queens), occ);

            queens &= (queens - 1);
        }

        // Add fields covered by the rooks
        field rooks = board.blackRook;
        while (rooks != 0) {
            threatened |= lookupSlider<piece::rook>(findFirst(rooks), occ);

            rooks &= (rooks - 1);
        }

        // Add fields covered by the bishops
        field bishops = board.blackBishop;
        while (bishops != 0) {
            threatened |= lookupSlider<piece::bishop>(findFirst(bishops), occ);
            bishops &= (bishops - 1);
        }

        // Add fields covered by the knights
        field knights = board.blackKnight;
        while (knights != 0) {
            threatened |= lookup<piece::knight>(findFirst(knights));
            knights &= (knights - 1);
        }

        // Add fields covered by pawns
        threatened |= (board.blackPawn & ~hFile) << 9;  // Taking to the left
        threatened |= (board.blackPawn & ~aFile) << 7;  // Taking to the right
    }

    return threatened;
}


template<bool color>
std::vector<t_gameState> generate_moves(t_gameState gameState) {
    /// THIS APPROACH WAS INSPIRED BY https://github.com/Gigantua/Gigantua ///

    std::vector<t_gameState> moves = std::vector<t_gameState>();

    t_board board = gameState.board;
    field occ = board.occupied;

    uint8_t whiteKingShift = findFirst(board.whiteKing);
    uint8_t blackKingShift = findFirst(board.blackKing);
    field whiteKingMap = board.whiteKing;
    field blackKingMap = board.blackKing;

    if (color) {
        // Black's turn

        // --------------------------- //
        // Generate threatened squares //
        // --------------------------- //

        field threatened = getThreatened<color>(board);


        // ----------------------------------------------------------- //
        // Generate squares causing checks their corresponding sliders //
        // ----------------------------------------------------------- //

        field checks = 0;
        field checkSliderPieces = 0;
        {
            field kingRookLookup = lookupSlider<piece::rook>(blackKingShift, occ);
            field kingBishopLookup = lookupSlider<piece::bishop>(blackKingShift, occ);
            field kingKnightLookup = lookup<piece::knight>(blackKingShift);

            if ((threatened & blackKingMap) != 0) {
                // There is at least one check

                // Check from sliding pieces
                checkSliderPieces |= kingRookLookup & board.whiteQueen;
                checkSliderPieces |= kingRookLookup & board.whiteRook;
                checkSliderPieces |= kingBishopLookup & board.whiteQueen;
                checkSliderPieces |= kingBishopLookup & board.whiteBishop;

                short checkSliderShift;
                field checkSliders = checkSliderPieces;  // Preserve value of checkSliderPieces
                while (checkSliders != 0) {
                    checkSliderShift = findFirst(checkSliders);
                    checks |= xray[64 * blackKingShift + checkSliderShift];

                    checkSliders &= (checkSliders - 1);
                }

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
                field kingTargets = lookup<piece::king>(blackKingShift) & ~threatened & ~board.black;
                moveKing<true>(&moves, gameState, blackKingMap, kingTargets);

                return moves;
            }
        }


        // --------------------------------------------------------- //
        // Generate squares causing pins their corresponding sliders //
        // --------------------------------------------------------- //

        field pinned;
        field lateralPins = 0;
        field diagonalPins = 0;
        {
            field kingVerticalMask = verticalMask[blackKingShift];
            field kingHorizontalMask = horizontalMask[blackKingShift];
            field kingLeftDiagonalMask = lDiagonalMask[blackKingShift];
            field kingRightDiagonalMask = rDiagonalMask[blackKingShift];

            field kingQueenPinLookup = lookupPinSlider<piece::queen>(blackKingShift, occ);
            field pinPieces =
                    kingQueenPinLookup & (board.whiteQueen | board.whiteRook | board.whiteBishop) & ~checkSliderPieces;

            field lateralPinShift;
            while (pinPieces != 0) {
                lateralPinShift = findFirst(pinPieces);
                field pinXray = xray[64 * blackKingShift + lateralPinShift];

                lateralPins |= pinXray & kingVerticalMask;
                lateralPins |= pinXray & kingHorizontalMask;
                diagonalPins |= pinXray & kingLeftDiagonalMask;
                diagonalPins |= pinXray & kingRightDiagonalMask;

                pinPieces &= (pinPieces - 1);
            }

            pinned = lateralPins | diagonalPins;
        }


        // --------------------------- //
        // Generate all possible moves //
        // --------------------------- //

        // Generate king moves
        {
            field kingTargets = lookup<piece::king>(blackKingShift) & ~threatened & ~board.black;
            moveKing<true>(&moves, gameState, blackKingMap, kingTargets);
        }


        // Generate castles
        {
            if (gameState.bCastleShort && (board.blackRook & hFile & rank8)) {
                if ((xray[453] & (occ | threatened | ~checks)) == 0) {  // Offset 453 = 64 * 7 + 5
                    moveKingCastleShort<true>(&moves, gameState);
                }
            }
            if (gameState.bCastleLong && (board.blackRook & aFile & rank8)) {
                if ((xray[3] & (occ | threatened | ~checks)) == 0) {  // Offset 3 = 64 * 0 + 3
                    moveKingCastleLong<true>(&moves, gameState);
                }
            }
        }


        // Generate queen moves
        {
            field queenOrigins = board.blackQueen & ~pinned;
            field queenOriginsPinnedLateral = board.blackQueen & lateralPins;
            field queenOriginsPinnedDiagonal = board.blackQueen & diagonalPins;

            short queenShift;
            field queenTargets;
            while (queenOrigins != 0) {
                queenShift = findFirst(queenOrigins);

                queenTargets = lookupSlider<piece::queen>(queenShift, occ) & checks & ~board.black;
                moveQueens<true>(&moves, gameState, queenShift, queenTargets);

                queenOrigins &= (queenOrigins - 1);
            }
            while (queenOriginsPinnedLateral != 0) {
                queenShift = findFirst(queenOriginsPinnedLateral);

                queenTargets = lookupSlider<piece::rook>(queenShift, occ) & checks & lateralPins & ~board.black;
                moveQueens<true>(&moves, gameState, queenShift, queenTargets);

                queenOriginsPinnedLateral &= (queenOriginsPinnedLateral - 1);
            }
            while (queenOriginsPinnedDiagonal != 0) {
                queenShift = findFirst(queenOriginsPinnedDiagonal);

                queenTargets = lookupSlider<piece::bishop>(queenShift, occ) & checks & diagonalPins & ~board.black;
                moveQueens<true>(&moves, gameState, queenShift, queenTargets);

                queenOriginsPinnedDiagonal &= (queenOriginsPinnedDiagonal - 1);
            }
        }


        // Generate rook moves
        {
            field rookOrigins = board.blackRook & ~pinned;
            field rookOriginsPinned = board.blackRook & lateralPins;  // Rooks pinned in diagonal pins can't move

            short rookShift;
            field rookTargets;
            while (rookOrigins != 0) {
                rookShift = findFirst(rookOrigins);

                rookTargets = lookupSlider<piece::rook>(rookShift, occ) & checks & ~board.black;
                moveRooks<true>(&moves, gameState, rookShift, rookTargets);

                rookOrigins &= (rookOrigins - 1);
            }
            while (rookOriginsPinned != 0) {
                rookShift = findFirst(rookOriginsPinned);

                rookTargets = lookupSlider<piece::rook>(rookShift, occ) & checks & lateralPins & ~board.black;
                moveRooks<true>(&moves, gameState, rookShift, rookTargets);

                rookOriginsPinned &= (rookOriginsPinned - 1);
            }
        }


        // Generate bishop moves
        {
            field bishopOrigins = board.blackBishop & ~pinned;
            field bishopOriginsPinned =
                    board.blackBishop & diagonalPins;  // Bishops pinned in lateral pins can't move

            short bishopShift;
            field bishopTargets;
            while (bishopOrigins != 0) {
                bishopShift = findFirst(bishopOrigins);

                bishopTargets = lookupSlider<piece::bishop>(bishopShift, occ) & checks & ~board.black;
                moveBishops<true>(&moves, gameState, bishopShift, bishopTargets);

                bishopOrigins &= (bishopOrigins - 1);
            }
            while (bishopOriginsPinned != 0) {
                bishopShift = findFirst(bishopOriginsPinned);

                bishopTargets = lookupSlider<piece::bishop>(bishopShift, occ) & checks & diagonalPins & ~board.black;
                moveBishops<true>(&moves, gameState, bishopShift, bishopTargets);

                bishopOriginsPinned &= (bishopOriginsPinned - 1);
            }
        }


        // Generate knight moves
        {
            field knightOrigins = board.blackKnight & ~pinned;

            short knightShift;
            field knightTargets;
            while (knightOrigins != 0) {
                knightShift = findFirst(knightOrigins);

                knightTargets = lookup<piece::knight>(knightShift) & checks & ~board.black;
                moveKnights<true>(&moves, gameState, knightShift, knightTargets);

                knightOrigins &= (knightOrigins - 1);
            }
        }


        // Generate pawn moves
        {
            field pawnTargets = ((board.blackPawn & ~diagonalPins) << 8) & checks & ~occ;

            field pawnTargetsPromotion = pawnTargets & rank1;
            pawnTargets &= ~pawnTargetsPromotion;

            field pawnOrigins = pawnTargets >> 8;
            field pawnOriginsPromotion = pawnTargetsPromotion >> 8;

            movePawns<true>(&moves, gameState, pawnOrigins, pawnTargets);
            movePawnsPromotion<true>(&moves, gameState, pawnOriginsPromotion, pawnTargetsPromotion);
        }


        // Generate pawn pushing moves
        {
            field pawnPushTargets =
                    ((((board.blackPawn & rank7 & ~diagonalPins) << 8) & ~occ) << 8) & checks & ~occ;
            field pawnPushOrigins = pawnPushTargets >> 16;

            movePawnsPush<true>(&moves, gameState, pawnPushOrigins, pawnPushTargets);
        }


        // Generate pawn taking moves
        {
            field pawnTakeRightTargets;
            pawnTakeRightTargets = ((board.blackPawn & ~aFile & ~pinned) << 7) & checks & board.white;
            pawnTakeRightTargets |=
                    ((board.blackPawn & ~aFile & diagonalPins) << 7) & checks & diagonalPins & board.white;

            field pawnTakeRightTargetsPromotion = pawnTakeRightTargets & rank1;
            pawnTakeRightTargets &= ~pawnTakeRightTargetsPromotion;

            field pawnTakeRightOrigins = pawnTakeRightTargets >> 7;
            field pawnTakeRightOriginsPromotion = pawnTakeRightTargetsPromotion >> 7;

            movePawns<true>(&moves, gameState, pawnTakeRightOrigins, pawnTakeRightTargets);
            movePawnsPromotion<true>(&moves, gameState, pawnTakeRightOriginsPromotion, pawnTakeRightTargetsPromotion);


            field pawnTakeLeftTargets;
            pawnTakeLeftTargets = ((board.blackPawn & ~hFile & ~pinned) << 9) & checks & board.white;
            pawnTakeLeftTargets |=
                    ((board.blackPawn & ~hFile & diagonalPins) << 9) & checks & diagonalPins & board.white;

            field pawnTakeLeftTargetsPromotion = pawnTakeLeftTargets & rank1;
            pawnTakeLeftTargets &= ~pawnTakeLeftTargetsPromotion;

            field pawnTakeLeftOrigins = pawnTakeLeftTargets >> 9;
            field pawnTakeLeftOriginsPromotion = pawnTakeLeftTargetsPromotion >> 9;

            movePawns<true>(&moves, gameState, pawnTakeLeftOrigins, pawnTakeLeftTargets);
            movePawnsPromotion<true>(&moves, gameState, pawnTakeLeftOriginsPromotion, pawnTakeLeftTargetsPromotion);
        }


        // Generate en-passants
        {
            // TODO: Approaches (left & right) not working for enpassant=8
            field pawnEnPassantRightTarget;  // There can only be one en-passant move per direction
            pawnEnPassantRightTarget =
                    ((board.blackPawn & ~aFile & (aFile << (gameState.enpassant)) & rank4 & ~pinned) << 7) & checks;
            pawnEnPassantRightTarget |=
                    ((board.blackPawn & ~aFile & (aFile << (gameState.enpassant)) & rank4 & diagonalPins) << 7) &
                    checks &
                    diagonalPins;
            field pawnEnPassantRightOrigins = pawnEnPassantRightTarget >> 7;

            movePawnsEnPassant<true>(&moves, gameState, pawnEnPassantRightOrigins, pawnEnPassantRightTarget);


            field pawnEnPassantLeftTarget;  // There can only be one en-passant move per direction
            pawnEnPassantLeftTarget =
                    ((board.blackPawn & ~hFile & (hFile >> (8 - gameState.enpassant)) & rank4 & ~pinned) << 9) & checks;
            pawnEnPassantLeftTarget |=
                    ((board.blackPawn & ~hFile & (hFile >> (8 - gameState.enpassant)) & rank4 & diagonalPins) << 9) & checks & diagonalPins;
            field pawnEnPassantLeftOrigins = pawnEnPassantLeftTarget >> 9;

            movePawnsEnPassant<true>(&moves, gameState, pawnEnPassantLeftOrigins, pawnEnPassantLeftTarget);
        }


        // TODO: Fix en-passant pin thingy
        int test = 0;
    } else {
        // White's turn

        // --------------------------- //
        // Generate threatened squares //
        // --------------------------- //

        field threatened = 0;
        {
            // Add fields covered by the king
            threatened |= lookup<piece::king>(blackKingShift);

            // Add fields covered by the queens
            field queens = board.blackQueen;
            while (queens != 0) {
                threatened |= lookupSlider<piece::queen>(findFirst(queens), occ);

                queens &= (queens - 1);
            }

            // Add fields covered by the rooks
            field rooks = board.blackRook;
            while (rooks != 0) {
                threatened |= lookupSlider<piece::rook>(findFirst(rooks), occ);

                rooks &= (rooks - 1);
            }

            // Add fields covered by the bishops
            field bishops = board.blackBishop;
            while (bishops != 0) {
                threatened |= lookupSlider<piece::bishop>(findFirst(bishops), occ);
                bishops &= (bishops - 1);
            }

            // Add fields covered by the knights
            field knights = board.blackKnight;
            while (knights != 0) {
                threatened |= lookup<piece::knight>(findFirst(knights));
                knights &= (knights - 1);
            }

            // Add fields covered by pawns
            threatened |= (board.blackPawn & ~aFile) << 9;  // Taking to the left
            threatened |= (board.blackPawn & ~hFile) << 7;  // Taking to the right

            // Remove all fields covered by black pieces
            threatened &= ~board.black;
        }


        // ----------------------------------------------------------- //
        // Generate squares causing checks their corresponding sliders //
        // ----------------------------------------------------------- //

        field checks = 0;
        field checkSliderPieces = 0;
        {
            field kingRookLookup = lookupSlider<piece::rook>(whiteKingShift, occ);
            field kingBishopLookup = lookupSlider<piece::bishop>(whiteKingShift, occ);
            field kingKnightLookup = lookup<piece::knight>(whiteKingShift);

            if ((threatened & whiteKingMap) != 0) {
                // There is at least one check

                // Check from sliding pieces
                checkSliderPieces |= kingRookLookup & board.blackQueen;
                checkSliderPieces |= kingRookLookup & board.blackRook;
                checkSliderPieces |= kingBishopLookup & board.blackQueen;
                checkSliderPieces |= kingBishopLookup & board.blackBishop;

                short checkSliderShift;
                field checkSliders = checkSliderPieces;  // Preserve value of checkSliderPieces
                while (checkSliders != 0) {
                    checkSliderShift = findFirst(checkSliders);
                    checks |= xray[64 * blackKingShift + checkSliderShift];

                    checkSliders &= (checkSliders - 1);
                }

                // Check from knights
                checks |= kingKnightLookup & board.blackKnight;

                // Check from pawns
                checks |= ((board.whiteKing & ~aFile) << 7) & board.blackPawn;  // Pawns threatening to the left
                checks |= ((board.whiteKing & ~hFile) << 9) & board.blackPawn;  // Pawns threatening to the right
            }

            // Handle different amounts of checking pieces
            field checkOrigins = checks & board.black;
            if (checkOrigins == 0) {
                // No checks -> Set all fields to 1
                checks = ~0;
            } else if ((checkOrigins & (checkOrigins - 1)) != 0) {
                // More than one check -> Only King can move
                field kingTargets = lookup<piece::king>(whiteKingShift) & ~threatened & ~board.white;
                moveKing<false>(&moves, gameState, whiteKingMap, kingTargets);

                return moves;
            }
        }


        // --------------------------------------------------------- //
        // Generate squares causing pins their corresponding sliders //
        // --------------------------------------------------------- //

        field pinned;
        field lateralPins = 0;
        field diagonalPins = 0;
        {
            field kingVerticalMask = verticalMask[whiteKingShift];
            field kingHorizontalMask = horizontalMask[whiteKingShift];
            field kingLeftDiagonalMask = lDiagonalMask[whiteKingShift];
            field kingRightDiagonalMask = rDiagonalMask[whiteKingShift];

            field kingQueenPinLookup = lookupPinSlider<piece::queen>(whiteKingShift, occ);
            field pinPieces =
                    kingQueenPinLookup & (board.blackQueen | board.blackRook | board.blackBishop) & ~checkSliderPieces;

            field lateralPinShift;
            while (pinPieces != 0) {
                lateralPinShift = findFirst(pinPieces);
                field pinXray = xray[64 * whiteKingShift + lateralPinShift];

                lateralPins |= pinXray & kingVerticalMask;
                lateralPins |= pinXray & kingHorizontalMask;
                diagonalPins |= pinXray & kingLeftDiagonalMask;
                diagonalPins |= pinXray & kingRightDiagonalMask;

                pinPieces &= (pinPieces - 1);
            }

            pinned = lateralPins | diagonalPins;
        }


        // --------------------------- //
        // Generate all possible moves //
        // --------------------------- //

        // Generate king moves
        {
            field kingTargets = lookup<piece::king>(whiteKingShift) & ~threatened & ~board.white;
            moveKing<false>(&moves, gameState, whiteKingMap, kingTargets);
        }


        // Generate castles
        {
            if (gameState.wCastleShort && (board.whiteRook & hFile & rank1)) {
                if ((xray[453] & (occ | threatened | ~checks)) == 0) {  // Offset 453 = 64 * 7 + 5
                    moveKingCastleShort<false>(&moves, gameState);
                }
            }
            if (gameState.wCastleLong && (board.whiteRook & aFile & rank1)) {
                if ((xray[3] & (occ | threatened | ~checks)) == 0) {  // Offset 3 = 64 * 0 + 3
                    moveKingCastleLong<false>(&moves, gameState);
                }
            }
        }


        // Generate queen moves
        {
            field queenOrigins = board.whiteQueen & ~pinned;
            field queenOriginsPinnedLateral = board.whiteQueen & lateralPins;
            field queenOriginsPinnedDiagonal = board.whiteQueen & diagonalPins;

            short queenShift;
            field queenTargets;
            while (queenOrigins != 0) {
                queenShift = findFirst(queenOrigins);

                queenTargets = lookupSlider<piece::queen>(queenShift, occ) & checks & ~board.white;
                moveQueens<false>(&moves, gameState, queenShift, queenTargets);

                queenOrigins &= (queenOrigins - 1);
            }
            while (queenOriginsPinnedLateral != 0) {
                queenShift = findFirst(queenOriginsPinnedLateral);

                queenTargets = lookupSlider<piece::rook>(queenShift, occ) & checks & lateralPins & ~board.white;
                moveQueens<false>(&moves, gameState, queenShift, queenTargets);

                queenOriginsPinnedLateral &= (queenOriginsPinnedLateral - 1);
            }
            while (queenOriginsPinnedDiagonal != 0) {
                queenShift = findFirst(queenOriginsPinnedDiagonal);

                queenTargets = lookupSlider<piece::bishop>(queenShift, occ) & checks & diagonalPins & ~board.white;
                moveQueens<false>(&moves, gameState, queenShift, queenTargets);

                queenOriginsPinnedDiagonal &= (queenOriginsPinnedDiagonal - 1);
            }
        }


        // Generate rook moves
        {
            field rookOrigins = board.whiteRook & ~pinned;
            field rookOriginsPinned = board.whiteRook & lateralPins;  // Rooks pinned in diagonal pins can't move

            short rookShift;
            field rookTargets;
            while (rookOrigins != 0) {
                rookShift = findFirst(rookOrigins);

                rookTargets = lookupSlider<piece::rook>(rookShift, occ) & checks & ~board.white;
                moveRooks<false>(&moves, gameState, rookShift, rookTargets);

                rookOrigins &= (rookOrigins - 1);
            }
            while (rookOriginsPinned != 0) {
                rookShift = findFirst(rookOriginsPinned);

                rookTargets = lookupSlider<piece::rook>(rookShift, occ) & checks & lateralPins & ~board.white;
                moveRooks<false>(&moves, gameState, rookShift, rookTargets);

                rookOriginsPinned &= (rookOriginsPinned - 1);
            }
        }


        // Generate bishop moves
        {
            field bishopOrigins = board.whiteBishop & ~pinned;
            field bishopOriginsPinned =
                    board.whiteBishop & diagonalPins;  // Bishops pinned in lateral pins can't move

            short bishopShift;
            field bishopTargets;
            while (bishopOrigins != 0) {
                bishopShift = findFirst(bishopOrigins);

                bishopTargets = lookupSlider<piece::bishop>(bishopShift, occ) & checks & ~board.white;
                moveBishops<false>(&moves, gameState, bishopShift, bishopTargets);

                bishopOrigins &= (bishopOrigins - 1);
            }
            while (bishopOriginsPinned != 0) {
                bishopShift = findFirst(bishopOriginsPinned);

                bishopTargets = lookupSlider<piece::bishop>(bishopShift, occ) & checks & diagonalPins & ~board.white;
                moveBishops<false>(&moves, gameState, bishopShift, bishopTargets);

                bishopOriginsPinned &= (bishopOriginsPinned - 1);
            }
        }


        // Generate knight moves
        {
            field knightOrigins = board.whiteKnight & ~pinned;

            short knightShift;
            field knightTargets;
            while (knightOrigins != 0) {
                knightShift = findFirst(knightOrigins);

                knightTargets = lookup<piece::knight>(knightShift) & checks & ~board.white;
                moveKnights<false>(&moves, gameState, knightShift, knightTargets);

                knightOrigins &= (knightOrigins - 1);
            }
        }


        // Generate pawn moves
        {
            field pawnTargets = ((board.whitePawn & ~diagonalPins) >> 8) & checks & ~occ;

            field pawnTargetsPromotion = pawnTargets & rank8;
            pawnTargets &= ~pawnTargetsPromotion;

            field pawnOrigins = pawnTargets << 8;
            field pawnOriginsPromotion = pawnTargetsPromotion << 8;

            movePawns<false>(&moves, gameState, pawnOrigins, pawnTargets);
            movePawnsPromotion<false>(&moves, gameState, pawnOriginsPromotion, pawnTargetsPromotion);
        }


        // Generate pawn pushing moves
        {
            field pawnPushTargets =
                    ((((board.white & rank2 & ~diagonalPins) >> 8) & ~occ) >> 8) & checks & ~occ;
            field pawnPushOrigins = pawnPushTargets << 16;

            movePawnsPush<false>(&moves, gameState, pawnPushOrigins, pawnPushTargets);
        }


        // Generate pawn taking moves
        {
            field pawnTakeRightTargets;
            pawnTakeRightTargets = ((board.whitePawn & ~hFile & ~pinned) >> 7) & checks & board.black;
            pawnTakeRightTargets |=
                    ((board.whitePawn & ~hFile & diagonalPins) >> 7) & checks & diagonalPins & board.black;

            field pawnTakeRightTargetsPromotion = pawnTakeRightTargets & rank8;
            pawnTakeRightTargets &= ~pawnTakeRightTargetsPromotion;

            field pawnTakeRightOrigins = pawnTakeRightTargets << 7;
            field pawnTakeRightOriginsPromotion = pawnTakeRightTargetsPromotion << 7;

            movePawns<false>(&moves, gameState, pawnTakeRightOrigins, pawnTakeRightTargets);
            movePawnsPromotion<false>(&moves, gameState, pawnTakeRightOriginsPromotion, pawnTakeRightTargetsPromotion);


            field pawnTakeLeftTargets;
            pawnTakeLeftTargets = ((board.whitePawn & ~aFile & ~pinned) >> 9) & checks & board.black;
            pawnTakeLeftTargets |=
                    ((board.whitePawn & ~aFile & diagonalPins) >> 9) & checks & diagonalPins & board.black;

            field pawnTakeLeftTargetsPromotion = pawnTakeLeftTargets & rank8;
            pawnTakeLeftTargets &= ~pawnTakeLeftTargetsPromotion;

            field pawnTakeLeftOrigins = pawnTakeLeftTargets << 9;
            field pawnTakeLeftOriginsPromotion = pawnTakeLeftTargetsPromotion << 9;

            movePawns<false>(&moves, gameState, pawnTakeLeftOrigins, pawnTakeLeftTargets);
            movePawnsPromotion<false>(&moves, gameState, pawnTakeLeftOriginsPromotion, pawnTakeLeftTargetsPromotion);
        }


        // Generate en-passants
        {
            field pawnEnPassantLeftTarget;  // There can only be one en-passant move per direction
            pawnEnPassantLeftTarget =
                    ((board.whitePawn & ~aFile & (aFile << (gameState.enpassant)) & rank5 & ~pinned) >> 9) & checks;
            pawnEnPassantLeftTarget |=
                    ((board.whitePawn & ~aFile & (aFile << (gameState.enpassant)) & rank5 & diagonalPins) >> 9) &
                    checks &
                    diagonalPins;
            field pawnEnPassantLeftOrigins = pawnEnPassantLeftTarget << 9;

            movePawnsEnPassant<false>(&moves, gameState, pawnEnPassantLeftOrigins, pawnEnPassantLeftTarget);


            field pawnEnPassantRightTarget;  // There can only be one en-passant move per direction
            pawnEnPassantRightTarget =
                    ((((board.blackPawn & ~hFile & ~pinned) << 1) & (hFile >> (8 - gameState.enpassant)) & rank5) >> 8) & checks;
            pawnEnPassantRightTarget |=
                    ((((board.blackPawn & ~hFile & diagonalPins) << 1) & (hFile >> (8 - gameState.enpassant)) & rank5) >> 8) & checks & diagonalPins;
            field pawnEnPassantRightOrigins = pawnEnPassantRightTarget << 7;

            movePawnsEnPassant<false>(&moves, gameState, pawnEnPassantRightOrigins, pawnEnPassantRightTarget);
        }


        // TODO: Fix en-passant pin thingy
    }

    return moves;
}


inline void printMove(t_gameState state) {
    t_move move = state.move;

    uint8_t originShift = (uint8_t )log2((double )move.originMap);
    uint8_t targetShift = (uint8_t )log2((double )move.targetMap);

    Position originPosition = position_from_shift(originShift);
    Position targetPosition = position_from_shift(targetShift);

    printf("Move %c%d -> %c%d",
           columnToLetter(originPosition.x), originPosition.y + 1,
           columnToLetter(targetPosition.x), targetPosition.y + 1);
}


#endif //KINGOFTHEHILL_KI_MOVE_H
