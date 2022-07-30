OS := $(shell uname -s)
ARCH := $(shell uname -m)

spork: main.cpp filter.o reader.o writer.o bidask.o
	g++ -std=c++17 main.cpp filter.o reader.o writer.o bidask.o -O1 -o spork -ltbb

filter.o: filter/filter.cpp spork.hpp
	g++ -std=c++17 -c filter/filter.cpp -O1 -o filter.o -ltbb

bidask.o: function/bidask.cpp spork.hpp
	g++ -std=c++17 -c function/bidask.cpp -O1 -o bidask.o -ltbb

reader.o: io/reader.cpp spork.hpp
	g++ -std=c++17 -c io/reader.cpp -O1 -o reader.o -ltbb

writer.o: io/writer.cpp spork.hpp
	g++ -std=c++17 -c io/writer.cpp -O1 -o writer.o -ltbb

.PHONY: clear
# clean all output data
clear:
	rm -rf *.csv
	rm -rf *.txt

.PHONY: clean
# clean all output program
clean:
	rm -rf spork
	rm -f *.o


.PHONY: test spork
test:
	./spork ../sample/20140401/MC33_All_20140401.csv MC33_20140401.csv BidAsk_MC33_20140401_DW.csv
