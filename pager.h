#pragma once

#include "page.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include<unistd.h>
#include<fcntl.h>


//pager is supposed to take serialized bytes and store them on disk, in a .db file

class Pager {
private:
  int fd_;
  uint32_t num_pages_;
  uint32_t free_head_;
  uint32_t free_count_;

public:
  //constructor and destructor
  Pager(const char* path);
  ~Pager();

  //prohibit copy and assignment
  Pager(const Pager&)=delete;
  Pager& operator=(const Pager&)= delete;

  //getter and setter for the page
  std::vector<std::byte> get_page(uint32_t page_num);
  void set_page(uint32_t page_num, std::vector<std::byte>& data);

  //misc utility
  uint32_t create_page();
  void free_page(uint32_t page_num);
  uint32_t num_pages() const;
};
