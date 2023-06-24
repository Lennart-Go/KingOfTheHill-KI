

#include "scoredMove.h"

scoredMove::scoredMove(float score, t_gameState* move) {
    _score = score;
    _move = move;
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

