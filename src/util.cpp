#include <stdexcept>

#include "util.h"



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
    if (!this->isWithinBounds()) {
        printf("toPosition out of bounds for values (%d, %d)\n", this->x, this->y);
    }

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

    return {x, y};
}


bool board_value_from_shift(uint64_t board, int shift) {
    if ((board & ((uint64_t )1 << shift)) != 0)
        return true;
    return false;
}


std::vector<Position> board_value_positions(uint64_t board) {
    std::vector<Position> positions;

    for (int shift = 0; shift < 64; ++shift) {
        if (board_value_from_shift(board, shift)) {
            positions.push_back(position_from_shift(shift));
        }
    }

    return positions;
}

char columnToLetter(int column) {
    return (char ) ('A' + column);
}

void printPosition(Position pos) {
    printf("%c%d", columnToLetter(pos.x), pos.y + 1);
}

int max(int num1, int num2) {
    if (num1 > num2) {
        return num1;
    }
    return num2;
}

float max(float num1, float num2) {
    if (num1 >= num2) {
        return num1;
    }
    return num2;
}

int min(int num1, int num2) {
    if (num1 < num2) {
        return num1;
    }
    return num2;
}

float min(float num1, float num2) {
    if (num1 <= num2) {
        return num1;
    }
    return num2;
}


int countFigure(uint64_t singleBoard) {
    int count = 0;
    for (int i = 0; i < 64; i++) {
        if (singleBoard & ((uint64_t) 1 << i)) {
            count++;
        }
    }
    return count;
}


int randn(int start, int stop) {
    // Initialize rng
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    srand((unsigned ) ts.tv_nsec);

    // Generate and return random number within specified bounds
    return (int )((rand() % (stop - start)) + start);
}
