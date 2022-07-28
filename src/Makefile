OS := $(shell uname -s)
ARCH := $(shell uname -m)

spork: main.cpp filter.o reader.o writer.o
	g++ -std=c++17 main.cpp filter.o reader.o writer.o -O1 -o spork -ltbb

filter.o: filter/filter.cpp spork.hpp
	g++ -std=c++17 -c filter/filter.cpp -O1 -o filter.o -ltbb

reader.o: io/reader.cpp spork.hpp
	g++ -std=c++17 -c io/reader.cpp -O1 -o reader.o -ltbb

writer.o: io/writer.cpp spork.hpp
	g++ -std=c++17 -c io/writer.cpp -O1 -o writer.o -ltbb

.PHONY: clean
# clean all output file
clean:
	rm -rf spork
	rm -f *.o
	rm -rf *.csv


.PHONY: test spork
test:
	./spork ../sample/20140401/MC32_All_20140401.csv MC32_20140401.csv
