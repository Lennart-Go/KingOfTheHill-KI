#ifndef KINGOFTHEHILL_KI_TRANSPOSITIONTABLE_H
#define KINGOFTHEHILL_KI_TRANSPOSITIONTABLE_H

#include <cstdint>
#include <map>

#include "move.h"


/*
typedef struct tableentry {
    uint64_t hash;
    t_move bestMove;
    float score;
    uint8_t vision; // how deep the search was from this position
    uint8_t age;
    tableentry* next;
}t_entry;

typedef struct table {
    t_entry* first;
    t_entry* last;
}t_table;

t_table* init_table();
void print_entry(t_entry* entry);
void print_table(t_table* table);
t_entry* init_entry(uint64_t hash, t_move bestMove, float score, uint8_t vision);
void add_entry(t_table* table, t_entry* newEntry);
t_entry* get_entry(t_table* table, uint64_t hash);
void remove_entry(t_table* table, t_entry* oldEntry);
void age_table(t_table* table);*/


class TableEntry{
public:
    TableEntry(uint64_t hash, t_move bestMove, float score, uint8_t vision);
    uint64_t getHash() const;
    void setHash(uint64_t hash);
    const t_move &getBestMove() const;
    void setBestMove(const t_move &bestMove);
    float getScore() const;
    void setScore(float score);
    uint8_t getVision() const;
    void setVision(uint8_t vision);
    long int getAge() const;

private:
    uint64_t _hash;
    t_move *_bestMove;
    float _score;
    uint8_t _vision;
    long int _age;
};

class TranspositionTable{
public:
    TranspositionTable();
    ~TranspositionTable();
    void print_entry(TableEntry entry);
    void removeEntry(TableEntry entry);
    TableEntry* getEntry(uint64_t hash);
    void setEntry(TableEntry te);
    int getSize() const;
    long int getAge() const;
    void setAge(long int age);
    void ageingTable();
    std::map<uint64_t,TableEntry>* _hashTable;
private:
    long int _entryCounter;
    int _currentAge;

};

#endif //KINGOFTHEHILL_KI_TRANSPOSITIONTABLE_H