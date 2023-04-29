#include "util.h"
#include <stdexcept>
#include <cstring>


template <typename T> class List {
private:
    T *_items;
    size_t _length = 0;

public:
    List() {
        this->_items = calloc(0, sizeof(T));
    }

    List(T *arr, size_t arrLength) {
        this->_items = calloc(arrLength, sizeof(T));
        memcpy(this->_items, arr, arrLength * sizeof(T));

        this->_length = arrLength;
    }

    void add(T item) {
        this->_items = reallocarray(this->_items, this->_length + 1, sizeof(T));
        this->_items[this->_length] = item;
        this->_length++;
    }

    T get(int index) const {
        if (abs(index) > (this->_length - 1)) {
            return NULL;
        }

        if (index < 0) {
            index = this->_length - index;
        }

        return this->_items[index];
    }

    T pop() {
        if (this->_length == 0) {
            return NULL;
        }

        T lastElement = this->_items[this->_length - 1];

        T *newItems = calloc(this->_length - 1, sizeof(T));
        memcpy(newItems, this->_items, (this->_length - 1) * sizeof(T));
        free(this->_items);

        this->_items = newItems;
        this->_length--;

        return lastElement;
    }

    size_t size() const {
        return (size_t) (this->_length * sizeof(T));
    }

    size_t length() const {
        return this->_length;
    }

    T *items() const {
        return this->_items;
    }
};


class Position {
public:
    unsigned x:3;
    unsigned y:3;

    Position() {
        this->x = 0;
        this->y = 0;
    }

    Position(int val1, int val2) {
        if (val1 < 0 || val1 > 7) {
            throw std::invalid_argument("Argument 1 outside bounds [0,7]");
        }
        if (val2 < 0 || val2 > 7) {
            throw std::invalid_argument("Argument 2 outside bounds [0,7]");
        }

        this->x = val1;
        this->y = val2;
    }

    Position operator+(const Position &other) const {
        Position nPosition;

        nPosition.x = this->x + other.x;
        nPosition.y = this->y + other.y;

        return nPosition;
    }

    Position operator-(const Position &other) const {
        Position nPosition;

        nPosition.x = this->x - other.x;
        nPosition.y = this->y - other.y;

        return nPosition;
    }

    Position operator*(const int multiplier) const {
        Position nPosition;

        nPosition.x = this->x * multiplier;
        nPosition.y = this->y * multiplier;

        return nPosition;
    }
};


class Offset {
public:
    unsigned x:6;
    unsigned y:6;

    Offset() {
        this->x = 0;
        this->y = 0;
    }

    Offset(int val1, int val2) {
        if (val1 < -7 || val1 > 7) {
            throw std::invalid_argument("Argument 1 outside bounds [-7,7]");
        }
        if (val2 < -7 || val2 > 7) {
            throw std::invalid_argument("Argument 2 outside bounds [-7,7]");
        }
        this->x = val1;
        this->y = val2;
    }

    Offset operator+(const Offset &other) const {
        Offset nOffset;

        nOffset.x = this->x + other.x;
        nOffset.y = this->y + other.y;

        return nOffset;
    }

    Offset operator+(const Position &other) const {
        Offset nOffset;

        nOffset.x = this->x + other.x;
        nOffset.y = this->y + other.y;

        return nOffset;
    }

    Offset operator-(const Offset &other) const {
        Offset nOffset;

        nOffset.x = this->x - other.x;
        nOffset.y = this->y - other.y;

        return nOffset;
    }

    Offset operator-(const Position &other) const {
        Offset nOffset;

        nOffset.x = this->x - other.x;
        nOffset.y = this->y - other.y;

        return nOffset;
    }

    Offset operator*(const int multiplier) const {
        Offset nOffset;

        nOffset.x = this->x * multiplier;
        nOffset.y = this->y * multiplier;

        return nOffset;
    }

    bool isWithinBounds() const {
        if (this->x < 0 || this->x > 7) {
            return false;
        }
        if (this->y < 0 || this->y > 7) {
            return false;
        }
        return true;
    }

    Position toPosition() const {
        // This ignores value constraints on Position objects

        return Position { (int) this->x, (int) this->y };
    }
};

int shift_from_position(Position position) {
    return 63 - (position.y * 8 + (7 - position.x));
}

