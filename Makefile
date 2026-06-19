CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

db: main.o page.o pager.o database.o
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp database.h
page.o: page.cpp page.h
pager.o: pager.cpp pager.h page.h
database.o: database.cpp database.h pager.h page.h

.PHONY: clean
clean:
	rm -f db *.o
