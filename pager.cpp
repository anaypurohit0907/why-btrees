#include "pager.h"
#include "page.h"
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>
#include <vector>

// constructor and destructor:
Pager::Pager(const char *db_path) {

  fd_ = open(db_path, O_RDWR | O_CREAT, 0644);
  if (fd_ == -1) {
    // return or exit with error saying that file opening failed
    throw std::runtime_error(
        "failed to open the file: " + std::string(db_path) +
        ". reason: " + std::strerror(errno));
  }
  off_t size = lseek(fd_, 0, SEEK_END);
  if (size == 0) {
    num_pages_ = 1;
    free_head_ = 0;
    free_count_ = 0;
    uint8_t buf[4096];
    std::memcpy(&buf[0], &PAGE_MAGIC, sizeof(PAGE_MAGIC));
    std::memcpy(&buf[4], &free_head_, sizeof(free_head_));
    std::memcpy(&buf[8], &free_count_, sizeof(free_count_));
    std::memcpy(&buf[12], &num_pages_, sizeof(num_pages_));

    pwrite(fd_, buf, 4096, 0);

  } else {
    uint8_t buf[4096];
    pread(fd_, buf, 4096, 0);
    uint32_t temp_magic;
    memcpy(&temp_magic, &buf[0], 4);
    if (temp_magic == PAGE_MAGIC) {
      memcpy(&free_head_, &buf[4], 4);
      memcpy(&free_count_, &buf[8], 4);
      memcpy(&num_pages_, &buf[12], 4);
    } else {
      throw std::runtime_error("magic number does not match, file is either "
                               "corrupted or not ours: " +
                               std::string(db_path));
    }
  }
}

Pager::~Pager() {
  if (fd_ >= 0) {
    close(fd_);
  }
}

// member variables:

std::vector<std::byte> Pager::get_page(uint32_t page_num) {
  std::vector<std::byte> buf(PAGE_SIZE);
  off_t offset = page_num * static_cast<off_t>(PAGE_SIZE);
  ssize_t n = pread(fd_, buf.data(), PAGE_SIZE, offset);
  if (n == -1) {
    throw std::runtime_error(" pread for page: " + std::to_string(page_num) +
                             " failed.");
  } else if (n != PAGE_SIZE) {

    throw std::runtime_error("partial read happend for page: " +
                             std::to_string(page_num));
  }

  return buf;
}

void Pager::set_page(uint32_t page_num, std::vector<std::byte> &data) {
  off_t offset = page_num * static_cast<off_t>(PAGE_SIZE);
  ssize_t n = pwrite(fd_, data.data(), PAGE_SIZE, offset);
  if (n == -1) {
    throw std::runtime_error(
        "write failed for page number: " + std::to_string(page_num) +
        ". reason: " + std::strerror(errno));

  } else if (n != PAGE_SIZE) {

    throw std::runtime_error("partial write happend for page number" +
                             std::to_string(page_num));
  }
}

uint32_t Pager::create_page() {
  if (free_head_ != 0) {
    uint32_t free_page = free_head_;
    ssize_t n =
        pread(fd_, &free_head_, 4, static_cast<off_t>(free_page * PAGE_SIZE));
    if (n == -1) {

      throw std::runtime_error(
          "read failed for page number: " + std::to_string(free_head_) +
          ". reason: " + std::strerror(errno));
    } else if (n != 4) {

      throw std::runtime_error("partial read happend for page number" +
                               std::to_string(free_head_));
    }
    free_count_--;
    uint8_t buf[8];
    std::memcpy(&buf, &free_head_, 4);
    std::memcpy(&buf[4], &free_count_, 4);
    ssize_t x = pwrite(fd_, &buf, 8, 4);
    if (x == -1) {
      throw std::runtime_error(
          "write failed for page number: " + std::to_string(free_page) +
          ". reason: " + std::strerror(errno));

    } else if (x != 8) {

      throw std::runtime_error("partial write happend for page number" +
                               std::to_string(free_page));
    }
    return free_page;

  } else {
    off_t offset = num_pages_ * PAGE_SIZE;

    if (ftruncate(fd_, offset + PAGE_SIZE) == -1) {
      throw std::runtime_error("failed to grow file to " +
                               std::to_string(offset + PAGE_SIZE) +
                               " bytes, reason: " + std::strerror(errno));
    }
    num_pages_++;
    free_head_ = 0;
    uint8_t buf[12];
    memcpy(&buf[0], &free_head_, 4);
    memcpy(&buf[4], &free_count_, 4);
    memcpy(&buf[8], &num_pages_, 4);
    pwrite(fd_, &buf[0], 12, 4);
    return num_pages_ - 1;
  }
}

void Pager::free_page(uint32_t page_num) {
  ssize_t n =
      pwrite(fd_, &free_head_, 4, static_cast<off_t>(page_num * PAGE_SIZE));
  if (n == -1) {
    throw std::runtime_error(
        "write failed for page number: " + std::to_string(page_num) +
        ". reason: " + std::strerror(errno));

  } else if (n != 4) {

    throw std::runtime_error("partial write happend for page number" +
                             std::to_string(page_num));
  }

  free_head_ = page_num;
  free_count_++;
  uint8_t buf[8];
  memcpy(&buf[0], &free_head_, 4);
  memcpy(&buf[4], &free_count_, 4);
  ssize_t x = pwrite(fd_, &buf[0], 8, 4);
  if (x == -1) {
    throw std::runtime_error(
        "write failed for header update while working on page number: " +
        std::to_string(page_num));

  } else if (x != 8) {

    throw std::runtime_error("partial write happend while updating header "
                             "while working on  page number: " +
                             std::to_string(page_num));
  }
}

uint32_t Pager::num_pages() const { return num_pages_; }
