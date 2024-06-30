#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "Hash/sha3.h"

class HashTable
{
private:
  std::vector<std::string> table;
  size_t table_size;

  size_t keccak256(const std::string &key);

public:
  HashTable(size_t size);
  void insert(const std::string &key, const std::string &value);
  std::string get(const std::string &key);
};
