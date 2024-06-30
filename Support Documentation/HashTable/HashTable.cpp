#include "HashTable.h"
#include "Hash/sha3.h"

HashTable::HashTable(size_t size) : table_size(size) {
    table.resize(size);
}

size_t HashTable::keccak256(const std::string& key) {
    sha3_context c;
    sha3_Init256(&c);
    sha3_Update(&c, key.c_str(), key.size());
    const uint8_t* hash = static_cast<const uint8_t*>(sha3_Finalize(&c));
    size_t hash_value = 0;
    for (int i = 0; i < 32; ++i) {
        hash_value = (hash_value << 8) | hash[i];
    }
    return hash_value % table_size;
}

void HashTable::insert(const std::string& key, const std::string& value) {
    size_t index = keccak256(key);
    table[index] = value;
}

std::string HashTable::get(const std::string& key) {
    size_t index = keccak256(key);
    return table[index];
}
