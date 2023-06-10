#include <inttypes.h>
#include "transpositionTable.h"

t_table* init_table() {
    t_table* table = (t_table*)calloc(1, sizeof(t_table));
    table->first = NULL;
    table->last = NULL;
    return table;
}

t_entry* init_entry(uint64_t hash, t_move bestMove, uint8_t vision) {
    t_entry* newEntry = (t_entry*)calloc(1, sizeof(t_entry));
    newEntry->hash = hash;
    newEntry->bestMove = bestMove;
    newEntry->vision = vision;
    newEntry->next = NULL;
    return newEntry;
}

void add_entry(t_table* table, t_entry* newEntry) {
    if (table->last == NULL) {
        table->first = newEntry;
        table->last = newEntry;
    }
    else {
        table->last->next = newEntry;
        table->last = newEntry;
    }
}

t_entry* get_entry(t_table* table, uint64_t hash) {
    t_entry* current = table->first;
    while (current != NULL) {
        if (current->hash == hash) return current;
        else current = current->next;
    }
    return NULL;
}

void remove_entry(t_table* table, t_entry* oldEntry) {
    t_entry* current = table->first;
    if (current == oldEntry) {
        table->first = oldEntry->next;
        if (current == table->last) table->last = oldEntry->next;
        free(oldEntry);
        return;
    }
    while (current->next != NULL) {
        if (current->next == oldEntry) {
            current->next = oldEntry->next;
            if (oldEntry->next == NULL) {
                table->last = current;
                free(oldEntry);
                return;
            }
            free(oldEntry);
            return;
        }
        else {
            current = current->next;
        }
    }
    throw std::runtime_error("Entry to remove not in table\n");
}

void print_entry(t_entry* entry) {
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("entry at: %p\n", entry);
    printf("------------------------------------\n");
    printf("hash: %" PRIu64 "\n", entry->hash);
    printf("best Move: ");
    printMove(entry->bestMove);
    printf("\n");
    printf("vision: %i\n", entry->vision);
    printf("next entry at: %p\n", entry->next);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void print_table(t_table* table) {
    t_entry* current = table->first;
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("START OF TABLE at %p\n", current);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    while (current != NULL) {
        print_entry(current);
        current = current->next;
    }
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("END OF TABLE\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}