#include <cstdlib>
#include <cstdint>


template <typename T> class List {
private:
    T *_items;
    size_t _length{};

public:
    List();
    List(T elem);
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
