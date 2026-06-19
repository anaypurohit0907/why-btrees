#include "database.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

int main() {
  Database db("./data.db");

  std::string line;
  std::string key, value, cmd;

  while (true) {
    std::cout << "> " << std::flush;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    iss >> cmd;
    iss >> std::ws;
    iss >> key;
    iss >> std::ws;
    std::getline(iss, value);

    if (cmd == "EXIT") {
      break;
    } else if (cmd == "GET") {
      try {
        std::cout << db.GET(key)<<"\n";
      } catch (const std::exception &e) {
        std::cout << "ERROR : " << e.what() << "\n";
      }

    } else if (cmd == "SET") {
      try {
        db.SET(key, value);
        std::cout << "OK\n";
      } catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << "\n";
      }
      }else{std::cout<<"Unknown command: "<<cmd<<"\n";}
  }

  return 0;
}
