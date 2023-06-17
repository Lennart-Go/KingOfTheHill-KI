#include <random>

#include "move.h"
#include "hash.h"


uint64_t* init_hash() {
    const int randomSize = 64 * 12;
    uint64_t* random = (uint64_t*)calloc(randomSize, sizeof(uint64_t));

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (int i = 0; i < randomSize; i++) {
        random[i] = dis(gen);
    }
    return random;
}

int getFigureOnPos(t_board board, int pos) {
    
    int offset;
    if ((board.white & ((uint64_t) 1 << pos)) >> pos) {
        offset = 0;
    }
    else if ((board.black & ((uint64_t) 1 << pos)) >> pos) {
        offset = OFFSET;
    }
    else return -1;

    if (((board.whiteKing | board.blackKing) & ((uint64_t) 1 << pos)) >> pos) {
        return KING + offset;
    }
    else if (((board.whiteQueen | board.blackQueen) & ((uint64_t) 1 << pos)) >> pos) {
        return QUEEN + offset;
    }
    else if (((board.whiteRook | board.blackRook) & ((uint64_t) 1 << pos)) >> pos) {
        return ROOK + offset;
    }
    else if (((board.whiteBishop | board.blackBishop) & ((uint64_t) 1 << pos)) >> pos) {
        return BISHOP + offset;
    }
    else if (((board.whiteKnight | board.blackKnight) & ((uint64_t) 1 << pos)) >> pos) {
        return KNIGHT + offset;
    }
    else if (((board.whitePawn | board.blackPawn) & ((uint64_t) 1 << pos)) >> pos) {
        return PAWN + offset;
    }
    return -1;
}

uint64_t hash(const uint64_t *random, t_gameState *state) {
    uint64_t hash = 0;

    for (int i = 0; i < 64; i++) {
        int figure = getFigureOnPos(state->board, i);
        if (figure != -1) hash ^= random[i*12+figure];
    }

    uint64_t stats = 0;
    stats |= (uint64_t) state->wCastleLong << 0;
    stats |= (uint64_t) state->wCastleShort << 1;
    stats |= (uint64_t) state->bCastleLong << 2;
    stats |= (uint64_t) state->bCastleShort << 3;
    stats |= (uint64_t) state->enpassant << 4;

    hash ^= stats;

    return hash;
}
