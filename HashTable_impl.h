#ifndef HASH_TABLE_IMPL_H
#define HASH_TABLE_IMPL_H

#include "HashTable.h"

using namespace std;

template <typename Key>
int djb2Hash(const Key& key) {
    unsigned long hash = 5381;
    for (char ch : key) {
        hash = ((hash << 5) + hash) + ch;  // hash * 33 + ch
    }
    return static_cast<int>(hash & 0x7FFFFFFF); // Убираем знак
}

// Функция для создания хэш-таблицы
template <typename Key, typename Value>
HashTable<Key, Value>* createHashTable(int initialCapacity, int expandThreshold) {
    if (initialCapacity <= 0 || expandThreshold <= 0 || expandThreshold > 100) {
        throw invalid_argument("Invalid capacity or threshold value.");
    }

    auto* table = new HashTable<Key, Value>;
    table->data = new HashNode<Key, Value>*[initialCapacity]();
    table->size = 0;
    table->capacity = initialCapacity;
    table->expandThreshold = expandThreshold;

    return table;
}

template <typename Key, typename Value>
void destroyHashTable(HashTable<Key, Value>& table) {
    clearHashTable(table);
    delete[] table.data;
    table.data = nullptr;
}

// Функция для расширения хэш-таблицы
template <typename Key, typename Value>
void expandHashTable(HashTable<Key, Value>& table) {
    int newCapacity = table.capacity * 2;
    auto newData = new HashNode<Key, Value>*[newCapacity]();

    cout << "Expanding table to new capacity: " << newCapacity << endl;

    for (int i = 0; i < table.capacity; ++i) {
        HashNode<Key, Value>* node = table.data[i];
        while (node) {
            HashNode<Key, Value>* nextNode = node->next;

            int newIndex = djb2Hash(node->key) % newCapacity;
            cout << "Rehashing key: " << node->key << " to new index: " << newIndex << endl;

            node->next = newData[newIndex];
            newData[newIndex] = node;

            node = nextNode;
        }
    }

    delete[] table.data;
    table.data = newData;
    table.capacity = newCapacity;

    cout << "Expansion complete." << endl;
}

// Функция для добавления данных в хэш-таблицу
template <typename Key, typename Value>
void setHash(HashTable<Key, Value>& table, const Key& key, const Value& value) {
    if ((table.size + 1) * 100 / table.capacity >= table.expandThreshold) {
        cout << "Expanding hash table..." << endl;
        expandHashTable(table);
    }

    int index = djb2Hash(key) % table.capacity;
    cout << "Adding key: " << key << " at index: " << index << endl;

    HashNode<Key, Value>* node = table.data[index];
    while (node) {
        if (node->key == key) {
            cout << "Key already exists. Updating value." << endl;
            node->value = value;
            return;
        }
        node = node->next;
    }

    auto* newNode = new HashNode<Key, Value>{key, value, table.data[index]};
    table.data[index] = newNode;
    ++table.size;

    cout << "Key added: " << key << " at index: " << index << endl;
}

// Функция для извлечения данных из хэш-таблицы
template <typename Key, typename Value>
Value getHash(const HashTable<Key, Value>& table, const Key& key) {
    int index = djb2Hash(key) % table.capacity;
    cout << "Looking for key: " << key << " at index: " << index << endl;

    HashNode<Key, Value>* node = table.data[index];
    while (node) {
        if (node->key == key) {
            cout << "Key found: " << key << endl;
            return node->value;
        }
        node = node->next;
    }

    cout << "Key not found: " << key << endl;
    throw runtime_error("Key not found.");
}

template <typename Key, typename Value>
void deleteHash(HashTable<Key, Value>& table, const Key& key) {
    int index = djb2Hash(key) % table.capacity;
    HashNode<Key, Value>* node = table.data[index];
    HashNode<Key, Value>* prev = nullptr;

    while (node) {
        if (node->key == key) {
            if (prev) {
                prev->next = node->next;
            } else {
                table.data[index] = node->next;
            }
            delete node;
            --table.size;
            return;
        }
        prev = node;
        node = node->next;
    }

    throw runtime_error("Key not found.");
}

template <typename Key, typename Value>
void clearHashTable(HashTable<Key, Value>& table) {
    for (int i = 0; i < table.capacity; ++i) {
        HashNode<Key, Value>* node = table.data[i];
        while (node) {
            HashNode<Key, Value>* nextNode = node->next;
            delete node;
            node = nextNode;
        }
        table.data[i] = nullptr;
    }
    table.size = 0;
}

template <typename Key, typename Value>
void replaceHash(HashTable<Key, Value>& table, const Key& key, const Value& newValue) {
    int index = djb2Hash(key) % table.capacity;
    HashNode<Key, Value>* node = table.data[index];

    while (node) {
        if (node->key == key) {
            node->value = newValue; // Обновляем значение
            return;
        }
        node = node->next;
    }

    throw runtime_error("Key not found."); // Если ключ отсутствует
}

#endif // HASH_TABLE_IMPL_H