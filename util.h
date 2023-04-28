#include <cstdlib>
#include <cstdint>


typedef struct linkedList {
    void *elem;
    linkedList *next;
} t_linkedList;


typedef struct countedList {
    void *list;
    size_t count;
} t_countedList;


typedef struct offset {
    signed x:6;
    signed y:6;
} t_offset;


typedef struct pos {
    unsigned x:3;
    unsigned y:3;
} t_pos;


bool is_within_bounds(t_offset offset);
bool shift_from_position(uint64_t board, t_pos position);
