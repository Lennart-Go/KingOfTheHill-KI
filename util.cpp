#include "util.h"
#include <stdexcept>


/* #############################################################################
 * ########################## List class definitions ###########################
 * #############################################################################
 */



/* #################################################################################
 * ########################## Position class definitions ###########################
 * #################################################################################
 */
Position::Position() {
    this->x = 0;
    this->y = 0;
}

Position::Position(int val1, int val2) {
    if (val1 < 0 || val1 > 7) {
        throw std::invalid_argument("Argument 1 outside bounds [0,7]");
    }
    if (val2 < 0 || val2 > 7) {
        throw std::invalid_argument("Argument 2 outside bounds [0,7]");
    }

    this->x = val1;
    this->y = val2;
}

Position Position::operator+(const Position &other) const {
    Position nPosition;

    nPosition.x = this->x + other.x;
    nPosition.y = this->y + other.y;

    return nPosition;
}

Position Position::operator-(const Position &other) const {
    Position nPosition;

    nPosition.x = this->x - other.x;
    nPosition.y = this->y - other.y;

    return nPosition;
}

Position Position::operator*(const int multiplier) const {
    Position nPosition;

    nPosition.x = this->x * multiplier;
    nPosition.y = this->y * multiplier;

    return nPosition;
}

bool Position::operator==(const Position &other) const {
    if ((this->x == other.x) && (this->y == other.y)) {
        return true;
    }
    return false;
}


/* ###############################################################################
 * ########################## Offset class definitions ###########################
 * ###############################################################################
 */
Offset::Offset() {
    this->x = 0;
    this->y = 0;
}

Offset::Offset(int val1, int val2) {
    if (val1 < -7 || val1 > 7) {
        throw std::invalid_argument("Argument 1 outside bounds [-7,7]");
    }
    if (val2 < -7 || val2 > 7) {
        throw std::invalid_argument("Argument 2 outside bounds [-7,7]");
    }
    this->x = val1;
    this->y = val2;
}

Offset Offset::operator+(const Offset &other) const {
    Offset nOffset;

    nOffset.x = this->x + other.x;
    nOffset.y = this->y + other.y;

    return nOffset;
}

Offset Offset::operator+(const Position &other) const {
    Offset nOffset;

    nOffset.x = this->x + other.x;
    nOffset.y = this->y + other.y;

    return nOffset;
}

Offset Offset::operator-(const Offset &other) const {
    Offset nOffset;

    nOffset.x = this->x - other.x;
    nOffset.y = this->y - other.y;

    return nOffset;
}

Offset Offset::operator-(const Position &other) const {
    Offset nOffset;

    nOffset.x = this->x - other.x;
    nOffset.y = this->y - other.y;

    return nOffset;
}

Offset Offset::operator*(const int multiplier) const {
    Offset nOffset;

    nOffset.x = this->x * multiplier;
    nOffset.y = this->y * multiplier;

    return nOffset;
}

bool Offset::operator==(const Offset &other) const {
    if ((this->x == other.x) && (this->y == other.y)) {
        return true;
    }
    return false;
}

bool Offset::operator==(const Position &other) const {
    if ((this->x == other.x) && (this->y == other.y)) {
        return true;
    }
    return false;
}

bool Offset::isWithinBounds() const {
    if (this->x < 0 || this->x > 7) {
        return false;
    }
    if (this->y < 0 || this->y > 7) {
        return false;
    }
    return true;
}

Position Offset::toPosition() const {
    // This ignores value constraints on Position objects

    return Position { (int) this->x, (int) this->y };
}


int shift_from_position(Position position) {
    return 63 - (position.y * 8 + (7 - position.x));
    // return 63 - (position.y * 8 + position.x);
}


Position position_from_shift(int shift) {
    int val = 63 - shift;

    int y = val / 8;
    int x = 7 - (val - (y * 8));
    // int x = val - (y * 8);

    return Position {x, y};
}


bool board_value_from_shift(uint64_t board, int shift) {
    if ((board & ((uint64_t) 1 << shift)) != 0)
        return true;
    return false;
}


List<Position> board_value_positions(uint64_t board) {
    List<Position> positions = List<Position>();

    for (int shift = 0; shift < 64; ++shift) {
        if (board_value_from_shift(board, shift)) {
            positions.add(position_from_shift(shift));
        }
    }

    return positions;
}

