#include "page.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <stdexcept>
#include <vector>
const int MAX_KEY_SIZE = 256;
const int MAX_VALUE_SIZE=3700;
Page::Page(){
	page_number_=0;
	magic_number_= PAGE_MAGIC;
	type_= Page_type::FREE;
	is_allocated_=false;
}

Page::Page(uint32_t page_number) : page_number_(page_number){
	magic_number_=PAGE_MAGIC;
	type_= Page_type::FREE;
	is_allocated_=false;
}


void Page::set_key(const std::string& key){
	if(key.size() > MAX_KEY_SIZE ){
		//error
		throw std::invalid_argument("key exceeds max size");
	}
	else{
		key_= key;
	}

}

std::string Page::get_key() const{
	return key_;
}

void Page::set_value(const std::string& value ){
	if(value.size() > MAX_VALUE_SIZE ){
		//error
		throw std::invalid_argument("value exceeds max size");
	}
	else{
		value_=value;
	}

}

std::string Page::get_value() const{
	return value_;
}

bool Page::is_valid() const{
	return magic_number_==PAGE_MAGIC;
}

bool Page::is_allocated(){
	return is_allocated_;
}

void Page::mark_free(){
	is_allocated_= false;
	type_= Page_type::FREE;
}

uint32_t Page::get_page_number() const{
	return page_number_;
}

std::vector<std::byte> Page::serialize() {

  std::vector<std::byte> data(PAGE_SIZE);
  // casting all the values
  uint8_t s_type = static_cast<uint8_t>(type_); 
  uint16_t key_length = static_cast<uint16_t>(key_.size());
  uint16_t value_length = static_cast<uint16_t>(value_.size());


  // copying the data
  std::memcpy(&data[0], &page_number_, sizeof(page_number_));
  std::memcpy(&data[4], &magic_number_, sizeof(magic_number_));
  std::memcpy(&data[8], &s_type, sizeof(s_type));
  std::memcpy(&data[9], &is_allocated_, sizeof(is_allocated_));
  std::memcpy(&data[10], &key_length, sizeof(key_length));
  std::memcpy(&data[12], key_.data(), key_length);
  std::memcpy(&data[12 + key_length], &value_length, sizeof(value_length));
  std::memcpy(&data[14 + key_length], value_.data(), value_length);

  return data;
}

void Page::deserialize(const std::vector<std::byte> &data) {
  // declaring variables
  uint32_t t_page_num;
  uint32_t t_magic_num;
  uint16_t key_len;
  uint16_t value_length;
  uint8_t t_is_allocated;
  uint8_t t_type;

  // copy the data
  memcpy(&t_page_num, &data[0], sizeof(t_page_num));
  page_number_ = t_page_num;
  memcpy(&t_magic_num, &data[4], sizeof(t_magic_num));
  magic_number_ = t_magic_num;
  memcpy(&t_type, &data[8], sizeof(t_type));
  type_ = static_cast<Page_type>(t_type);
  memcpy(&t_is_allocated, &data[9], sizeof(t_is_allocated));
  is_allocated_ = t_is_allocated;
  memcpy(&key_len, &data[10], sizeof(key_len));
  memcpy(&value_length, &data[12 + key_len], sizeof(value_length));
  key_.assign(reinterpret_cast<const char *>(&data[12]), key_len);
  value_.assign(reinterpret_cast<const char *>(&data[14 + key_len]),value_length);
}
