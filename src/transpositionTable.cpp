#include "transpositionTable.h"

/*t_table* init_table() {
    t_table* table = (t_table*)calloc(1, sizeof(t_table));
    table->first = NULL;
    table->last = NULL;
    return table;
}

t_entry* init_entry(uint64_t hash, t_move bestMove, float score, uint8_t vision) {
    t_entry* newEntry = (t_entry*)calloc(1, sizeof(t_entry));
    newEntry->hash = hash;
    newEntry->bestMove = bestMove;
    newEntry->age = 0;
    newEntry->vision = vision;
    newEntry->score = score;
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

void age_table(t_table* table) {
    t_entry* current = table->first;
    while (current != NULL) {
        current->age += 1;
        current = current->next;
    }
}

void print_entry(t_entry* entry) {
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("entry at: %p\n", entry);
    printf("------------------------------------\n");
    printf("hash: %" PRIu64 "\n", entry->hash);
    printf("best Move: ");
    printMove(entry->bestMove);
    printf("\n");
    printf("score: %f\n", entry->score);
    printf("vision: %i\n", entry->vision);
    printf("age: %hhu\n", entry->age);
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
}*/

TableEntry::TableEntry(uint64_t hash, t_move bestMove, float score, uint8_t vision) {
    _hash = hash;

    _bestMove = static_cast<t_move *>(calloc(1, sizeof(t_move)));
    memcpy(_bestMove, &bestMove, sizeof(t_move));

    //_score = static_cast<float *>(calloc(1, sizeof(float)));
    //memcpy(_score, &score, sizeof(float));
    _score = score;
    _vision = vision;
    //_age = age;
}

uint64_t TableEntry::getHash() const {
    return _hash;
}

void TableEntry::setHash(uint64_t hash) {
    _hash = hash;
}

const t_move &TableEntry::getBestMove() const {
    return *_bestMove;
}

void TableEntry::setBestMove(const t_move &bestMove) {
    memcpy(_bestMove, &bestMove, sizeof(t_move));
}

float TableEntry::getScore() const {
    return _score;
}

void TableEntry::setScore(float score) {
    _score = score;
    //memcpy(_score, &score, sizeof(float));
}

uint8_t TableEntry::getVision() const {
    return _vision;
}

void TableEntry::setVision(uint8_t vision) {
    _vision = vision;
}

long int TableEntry::getAge() const {
    return _age;
}

TranspositionTable::TranspositionTable() {
    _entryCounter = 0;
    _currentAge = 0;
    _hashTable = new std::map<uint64_t,TableEntry>();
}

TranspositionTable::~TranspositionTable() {
    delete _hashTable;
};



void TranspositionTable::removeEntry(TableEntry entry) {
    (*_hashTable).erase(entry.getHash());
    _entryCounter -= 1;
}

//return pointer to TableEntry if exists. If not returns NULL
TableEntry* TranspositionTable::getEntry(uint64_t hash) {
    if((*_hashTable).find(hash) != (*_hashTable).end()){
        return &(*_hashTable).find(hash)->second;
    }else{
        return nullptr;
    }
}

void TranspositionTable::setEntry(TableEntry te) {
    if((*_hashTable).find(te.getHash()) == (*_hashTable).end()) {
        (*_hashTable).insert(std::pair<uint64_t, TableEntry>(te.getHash(), te));
        _entryCounter++;
    }else {
        (*_hashTable).find(te.getHash())->second = te;
    }
}

int TranspositionTable::getSize() const {
    return (int )_entryCounter;
}

long int TranspositionTable::getAge() const {
    return _currentAge;
}

void TranspositionTable::setAge(long int age) {
    _currentAge = (int )age;
}

void TranspositionTable::ageingTable(){
    _currentAge++;
}



