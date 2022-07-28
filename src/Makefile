OS := $(shell uname -s)
ARCH := $(shell uname -m)

spork: main.cpp filter.o reader.o
	g++ -std=c++17 main.cpp filter.o reader.o -O1 -o spork -ltbb

filter.o: filter/filter.cpp spork.hpp
	g++ -std=c++17 -c filter/filter.cpp -O1 -o filter.o -ltbb

reader.o: filter/reader.cpp spork.hpp
	g++ -std=c++17 -c filter/reader.cpp -O1 -o reader.o -ltbb

.PHONY: clear
# clear all .o file
clear:
	rm -f filter.o
	rm -f reader.o

.PHONY: clean
# clean all output file
clean:
	rm -rf spork


.PHONY: test spork
test:
	./spork ../sample/20140401/MC32_All_20140401.csv
