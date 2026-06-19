#include "database.h"
#include "page.h"
#include "pager.h"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>







static void scan_pages(Pager& pager, std::unordered_map<std::string, uint32_t>& index){
	uint32_t num_pages=pager.num_pages();
	std::vector<std::byte> data;
	Page page;

	for(uint32_t i=1;i<num_pages;++i){
		data= pager.get_page(i);
		page.deserialize(data);
		if(page.is_valid() && page.is_allocated()){
			index[page.get_key()]=page.get_page_number();
		}
	}

	

}



Database::Database(const char* db_path ): pager_(db_path){
	scan_pages(pager_,index_);
}

  std::string Database::GET(const std::string &key){
	  Page page;
auto it = index_.find(key);
	  if(it != index_.end()){
			std::vector<std::byte> data= pager_.get_page(it->second);
			page.deserialize(data);
			return page.get_value();
	  }
	  else{throw std::invalid_argument("key not found");}

  }
  void Database::SET(const std::string &key, const std::string &value){
		  auto it = index_.find(key);
	  if(it != index_.end()){

		  Page page(it->second);
		  page.set_key(it->first);
		  page.set_value(value);
		  std::vector<std::byte>data=page.serialize();
		  pager_.set_page(it->second, data);
	  }
	  else{
		  uint32_t new_page= pager_.create_page();
		  Page page(new_page);
		 page.set_key(key);
		 page.set_value(value);
		 std::vector<std::byte>data=page.serialize();
		 pager_.set_page(page.get_page_number(), data);
		 index_[key]=new_page;
	  }
  }





