#include "util.h"
#include <stdexcept>
#include <cstring>


/* #############################################################################
 * ########################## List class definitions ###########################
 * #############################################################################
 */
template <typename T> List<T>::List() {
    this->_items = (T *) calloc(0, sizeof(T));
    this->_length = 0;
}

template <typename T> List<T>::List(T elem) {
    this->_items = (T *) calloc(1, sizeof(T));
    this->_length = 1;
}

template <typename T> List<T>::List(T *arr, size_t arrLength) {
    this->_items = (T *) calloc(arrLength, sizeof(T));
    memcpy(this->_items, arr, arrLength * sizeof(T));

    this->_length = arrLength;
}

template <typename T> List<T>::~List() {
    free(this->_items);
}

template <typename T> void List<T>::add(T item) {
    this->_items = (T *) reallocarray(this->_items, this->_length + 1, sizeof(T));
    this->_items[this->_length] = item;
    this->_length++;
}

template <typename T> T List<T>::get(int index) const {
    if (abs(index) > (this->_length - 1)) {
        // TODO: Raise out of bounds exception?
        return NULL;
    }

    if (index < 0) {
        index = this->_length - index;
    }

    return this->_items[index];
}

template <typename T> void List<T>::remove(int index) {
    if (abs(index) > (this->_length - 1)) {
        // TODO: Raise out of bounds exception?
        return;
    }

    T *newItems = (T *) calloc(this->_length - 1, sizeof(T));
    memcpy(newItems, this->_items, index * sizeof(T));  // Copy everything up to the selected element
    memcpy(newItems[index], this->_items[index+1], (this->_length-index-1) * sizeof(T));
    free(this->_items);

    this->_items = newItems;
    this->_length--;
}

template <typename T> T List<T>::pop() {
    if (this->_length == 0) {
        return NULL;
    }

    T lastElement = this->_items[this->_length - 1];

    T *newItems = (T *) calloc(this->_length - 1, sizeof(T));
    memcpy(newItems, this->_items, (this->_length - 1) * sizeof(T));
    free(this->_items);

    this->_items = newItems;
    this->_length--;

    return lastElement;
}

template <typename T> size_t List<T>::size() const {
    return (size_t) (this->_length * sizeof(T));
}

template <typename T> size_t List<T>::length() const {
    return this->_length;
}

template <typename T> bool List<T>::empty() const {
    return this->_length == 0;
}

template <typename T> T* List<T>::items() const {
    return this->_items;
}


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
}


Position position_from_shift(int shift) {
    int val = 63 - shift;

    int y = val / 8;
    int x = 7 - (val - (y * 8));

    return Position {x, y};
}


bool board_value_from_shift(uint64_t board, int shift) {
    if ((board & 1 << shift ) != 0)
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

