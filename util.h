#ifndef KINGOFTHEHILL_KI_UTIL_H
#define KINGOFTHEHILL_KI_UTIL_H

#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <cstring>

template <typename T> class List {
private:
    T *_items;
    size_t _length{};

public:
    List();
    explicit List(T elem);
    List(T *arr, size_t arrLength);
    virtual ~List();

    void add(T item);
    T get(int index) const;
    void remove(int index);
    T pop();

    size_t size() const;
    size_t length() const;
    bool empty() const;

    T *items() const;
};

template <typename T> List<T>::List() {
    this->_items = (T *) calloc(0, sizeof(T));
    this->_length = 0;
}

template <typename T> List<T>::List(T elem) {
    this->_items = (T *) calloc(1, sizeof(T));
    this->_items[0] = elem;
    this->_length = 1;
}

template <typename T> List<T>::List(T *arr, size_t arrLength) {
    this->_items = (T *) calloc(arrLength, sizeof(T));
    memcpy(this->_items, arr, arrLength * sizeof(T));

    this->_length = arrLength;
}

template <typename T> List<T>::~List() {
    if (this->_length < 1) {
        return;
    }

    // free(this->_items);
}

template <typename T> void List<T>::add(T item) {
    this->_items = (T *) realloc(this->_items, (this->_length + 1) * sizeof(T));

    T localItem;
    memcpy(&localItem, &item, sizeof(T));

    this->_items[this->_length] = localItem;
    this->_items[this->_length] = item;
    this->_length++;
}

template <typename T> T List<T>::get(int index) const {
    if (abs(index) > (this->_length - 1)) {
        // TODO: Raise out of bounds exception?
        throw std::out_of_range("Index out of range for List");
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
    memcpy(&newItems[index], &this->_items[index+1], (this->_length-index-1) * sizeof(T));
    free(this->_items);

    this->_items = newItems;
    this->_length--;
}

template <typename T> T List<T>::pop() {
    if (this->_length == 0) {
        printf("List index out of range!\n");
        throw std::out_of_range("Index out of range for List");
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
    unsigned x:6;
    unsigned y:6;

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

bool board_value_from_shift(uint64_t board, int shift);
List<Position> board_value_positions(uint64_t board);

char columnToLetter(int column);

#endif
