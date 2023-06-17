#include <cstdlib>
#include <random>
#include "game.h"
#include "hash.h"

uint64_t* init_hash() {
    const int radomSize = 64*12;
    uint64_t* random = (uint64_t*)calloc(radomSize, sizeof(uint64_t));

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    for (int i = 0; i < radomSize; i++) {
        random[i] = dis(gen);
    }
    return random;
}

int getFigureOnPos(t_game* game, int pos) {
    int offset;
    if ((game->board->white & ((uint64_t) 1 << pos)) >> pos) {
        offset = 0;
    }
    else if ((game->board->black & ((uint64_t) 1 << pos)) >> pos) {
        offset = OFFSET;
    }
    else return -1;

    if ((game->board->king & ((uint64_t) 1 << pos)) >> pos) {
        return KING + offset;
    }
    else if ((game->board->queen & ((uint64_t) 1 << pos)) >> pos) {
        return QUEEN + offset;
    }
    else if ((game->board->rook & ((uint64_t) 1 << pos)) >> pos) {
        return ROOK + offset;
    }
    else if ((game->board->bishop & ((uint64_t) 1 << pos)) >> pos) {
        return BISHOP + offset;
    }
    else if ((game->board->knight & ((uint64_t) 1 << pos)) >> pos) {
        return KNIGHT + offset;
    }
    else if ((game->board->pawn & ((uint64_t) 1 << pos)) >> pos) {
        return PAWN + offset;
    }
    return -1;
}

uint64_t hash(uint64_t* random, t_game* game) {

    uint64_t hash = 0;

    for (int i = 0; i < 64; i++) {
        int figure = getFigureOnPos(game, i);
        if (figure != -1) hash ^= random[i*12+figure];
    }

    uint64_t stats = 0;
    stats |= (uint64_t) game->whiteCanCastleLong << 0;
    stats |= (uint64_t) game->whiteCanCastleShort << 1;
    stats |= (uint64_t) game->whiteCastled << 2;
    stats |= (uint64_t) game->blackCanCastleLong << 3;
    stats |= (uint64_t) game->blackCanCastleShort << 4;
    stats |= (uint64_t) game->blackCastled << 5;
    stats |= (uint64_t) game->enpassants << 6;

    hash ^= stats;

    return hash;
}
