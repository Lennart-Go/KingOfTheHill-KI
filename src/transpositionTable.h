#ifndef KINGOFTHEHILL_KI_TRANSPOSITIONTABLE_H
#define KINGOFTHEHILL_KI_TRANSPOSITIONTABLE_H

#include <cstdint>
#include "move.h"

typedef struct tableentry {
    uint64_t hash;
    t_move bestMove;
    uint8_t vision; // how deep the search was from this position
    tableentry* next;
}t_entry;

typedef struct table {
    t_entry* first;
    t_entry* last;
}t_table;

t_table* init_table();
void print_entry(t_entry* entry);
void print_table(t_table* table);
t_entry* init_entry(uint64_t hash, t_move bestMove, uint8_t vision);
void add_entry(t_table* table, t_entry* newEntry);
t_entry* get_entry(t_table* table, uint64_t hash);
void remove_entry(t_table* table, t_entry* oldEntry);

#endif //KINGOFTHEHILL_KI_TRANSPOSITIONTABLE_H
