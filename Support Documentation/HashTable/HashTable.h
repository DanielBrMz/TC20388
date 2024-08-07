#pragma once

#include <vector>
#include <list>
#include <string>

template<typename K, typename V>
class HashTable {
private:
    static const int INITIAL_SIZE = 10;
    static const double LOAD_FACTOR = 0.80;

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
