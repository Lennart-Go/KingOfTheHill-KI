

#include "scoredMove.h"

scoredMove::scoredMove(float score, t_gameState* move) {
    _score = score;
    _move = static_cast<t_gameState *>(calloc(1, sizeof(t_gameState)));
    memcpy(_move, move, sizeof(t_gameState));
}

bool scoredMove::operator <(const scoredMove& a){
    return _score < a._score;
}

bool scoredMove::operator >(const scoredMove& a){
    return _score > a._score;
}

bool scoredMove::operator ==(const scoredMove& a){
    return _score == a._score;
}

scoredMove::~scoredMove() {
    free(_move);
}

