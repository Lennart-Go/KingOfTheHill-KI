//
// Created by user on 24.06.23.
//


#ifndef KINGOFTHEHILL_KI_SCOREDMOVE_H
#define KINGOFTHEHILL_KI_SCOREDMOVE_H

#include "move.h"

class scoredMove{
public:
    t_gameState* _move;
    float _score;
    scoredMove(float score, t_gameState* move);
    bool operator <(const scoredMove& a);
    bool operator >(const scoredMove& a);
    bool operator ==(const scoredMove& a);
};

#endif //KINGOFTHEHILL_KI_SCOREDMOVE_H
