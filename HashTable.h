#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <stdexcept>

template <typename Key, typename Value>
struct HashNode {
    Key key;
    Value value;
    HashNode* next; 
};

template <typename Key, typename Value>
struct HashTable {
    HashNode<Key, Value>** data;
    int size;                    
    int capacity;                
    int expandThreshold;         
};

template <typename Key>
int djb2Hash(const Key& key);

template <typename Key, typename Value>
HashTable<Key, Value>* createHashTable(int initialCapacity = 10, int expandThreshold = 75);

template <typename Key, typename Value>
void destroyHashTable(HashTable<Key, Value>& table);

template <typename Key, typename Value>
void setHash(HashTable<Key, Value>& table, const Key& key, const Value& value);

template <typename Key, typename Value>
Value getHash(const HashTable<Key, Value>& table, const Key& key);

template <typename Key, typename Value>
void deleteHash(HashTable<Key, Value>& table, const Key& key);

template <typename Key, typename Value>
void clearHashTable(HashTable<Key, Value>& table);

template <typename Key, typename Value>
void replaceHash(HashTable<Key, Value>& table, const Key& key, const Value& newValue);

#include "HashTable_impl.h"

#endif // HASH_TABLE_H