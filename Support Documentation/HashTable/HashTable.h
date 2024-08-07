#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <list>
#include <string>
#include <cmath>

template<typename K, typename V>
class HashTable {
private:
    static const int INITIAL_SIZE = 10;
    static constexpr double LOAD_FACTOR = 0.80;

    std::vector<std::list<std::pair<K, V>>> table;
    int size;
    int count;

    int hash(const K& key) const;
    void resize();

public:
    HashTable();
    void insert(const K& key, const V& value);
    bool remove(const K& key);
    bool get(const K& key, V& value) const;
    bool update(const K& key, const V& value);
    int getSize() const;
    int getCount() const;
    double getCurrentLoadFactor() const;
};

// Implementation

template<typename K, typename V>
HashTable<K, V>::HashTable() : size(INITIAL_SIZE), count(0) {
    table.resize(size);
}

template<typename K, typename V>
int HashTable<K, V>::hash(const K& key) const {
    return std::hash<K>{}(key) % size;
}

template<typename K, typename V>
void HashTable<K, V>::resize() {
    int newSize = size * 2;
    std::vector<std::list<std::pair<K, V>>> newTable(newSize);

    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            int newIndex = std::hash<K>{}(pair.first) % newSize;
            newTable[newIndex].push_back(pair);
        }
    }

    table = std::move(newTable);
    size = newSize;
}

template<typename K, typename V>
void HashTable<K, V>::insert(const K& key, const V& value) {
    if (getCurrentLoadFactor() >= LOAD_FACTOR) {
        resize();
    }

    int index = hash(key);
    for (auto& pair : table[index]) {
        if (pair.first == key) {
            pair.second = value;
            return;
        }
    }

    table[index].emplace_back(key, value);
    count++;
}

template<typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    int index = hash(key);
    auto& bucket = table[index];

    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            count--;
            return true;
        }
    }

    return false;
}

template<typename K, typename V>
bool HashTable<K, V>::get(const K& key, V& value) const {
    int index = hash(key);
    const auto& bucket = table[index];

    for (const auto& pair : bucket) {
        if (pair.first == key) {
            value = pair.second;
            return true;
        }
    }

    return false;
}

template<typename K, typename V>
bool HashTable<K, V>::update(const K& key, const V& value) {
    int index = hash(key);
    auto& bucket = table[index];

    for (auto& pair : bucket) {
        if (pair.first == key) {
            pair.second = value;
            return true;
        }
    }

    return false;
}

template<typename K, typename V>
int HashTable<K, V>::getSize() const {
    return size;
}

template<typename K, typename V>
int HashTable<K, V>::getCount() const {
    return count;
}

template<typename K, typename V>
double HashTable<K, V>::getCurrentLoadFactor() const {
    return static_cast<double>(count) / size;
}

#endif // HASH_TABLE_H