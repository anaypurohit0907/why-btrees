#pragma once

#include "pager.h"
#include <cstdint>
#include <string>
#include <unordered_map>
class Database {
private:
  std::unordered_map<std::string, uint32_t> index_;
  Pager pager_;

public:
  // constructor destructor
  Database(const char *db_path);
  ~Database()=default;
  // prevent copy and assignment
  Database(const Database &) = delete;
  Database &operator=(const Database &) = delete;

  // getter and setter
  std::string GET(const std::string &key);
  void SET(const std::string &key, const std::string &value);
};
