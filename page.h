#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>



const std::size_t PAGE_SIZE = 4096;
const uint32_t PAGE_MAGIC= 0xDEADBEEF;


enum class Page_type {
	DATA=1,
	FREE=0
};

class Page {
public:
	//constructor
	Page();
	explicit Page(uint32_t page_number);

	//key value methods
	void set_key(const std::string& key);
	std::string get_key() const;

	void set_value(const std::string& value );
	std::string get_value() const;

	bool is_valid() const;
	bool is_allocated();
	void mark_free();
	
	std::vector<std::byte> serialize();
	void deserialize(const std::vector<std::byte>&);

	uint32_t get_page_number() const;

private:
	uint32_t page_number_;
	uint32_t magic_number_;
	Page_type type_;
	std::string key_;
	std::string value_;
	bool is_allocated_=false;
	

};





