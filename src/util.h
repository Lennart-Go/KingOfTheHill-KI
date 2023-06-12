#include <vector>

#ifndef KINGOFTHEHILL_KI_UTIL_H
#define KINGOFTHEHILL_KI_UTIL_H

#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <cstring>

#define field uint64_t


class Position {
public:
    unsigned x:3;
    unsigned y:3;

    Position();
    Position(int val1, int val2);

    Position operator+(const Position &other) const;
    Position operator-(const Position &other) const;
    Position operator*(int multiplier) const;
    bool operator==(const Position &other) const;
};

class Offset {
public:
    signed x:7;
    signed y:7;

    Offset();
    Offset(int val1, int val2);

    Offset operator+(const Offset &other) const;
    Offset operator+(const Position &other) const;
    Offset operator-(const Offset &other) const;
    Offset operator-(const Position &other) const;
    Offset operator*(int multiplier) const;
    bool operator==(const Offset &other) const;
    bool operator==(const Position &other) const;

    bool isWithinBounds() const;

    Position toPosition() const;
};

int shift_from_position(Position position);
Position position_from_shift(int shift);

bool board_value_from_shift(field board, int shift);
std::vector<Position> board_value_positions(field board);

char columnToLetter(int column);
void printPosition(Position pos);

int max(int num1, int num2);
float max(float num1, float num2);
int min(int num1, int num2);
float min(float num1, float num2);

#endif
