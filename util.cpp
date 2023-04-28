#include "util.h"


bool is_within_bounds(const t_offset offset) {
    if (offset.x < 0 || offset.x > 7)
        return false;
    if (offset.y < 0 || offset.y > 7)
        return false;
    return true;
}

int shift_from_position(uint64_t board, t_pos position) {
    return 63 - (position.y * 8 + (7 - position.x));
}

