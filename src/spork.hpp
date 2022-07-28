//===----------------------------------------------------------------------===//
//                         spork
//
// spork
//
// Identification: src/filter/spork.hpp
//
// Last Modified : 2022.7.28 Jiawei Wang
//
// Copyright (c) 2022 Angold-4
//
//===----------------------------------------------------------------------===//


/**
 * STOCK CODE ALLOCATION PLAN FOR DERIVATIVE WARRANTS
 * AND CALLABLE BULL / BEAR CONTRACTS:
 *
 * DW   security code range: 10000 - 29999
 * CBBC security code range: 50000 - 69999
 */


#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#define DEBUG 1
#define DWT 1
#define DWLB 10000
#define DWUB 29999
#define CBBCT 2
#define CBBCLB 50000
#define CBBCUB 69999
#define VI std::vector<int>
#define VS std::vector<std::string>
#define VLLS std::vector<std::pair<long long, security>> 

using ll = long long;

namespace spork {

struct security {
  std:: string date;
  std::string code;
  bool ad; // T: add, F: delete
  std::string orderId;
  bool bidask; // T: bid, F: ask
  double price;
  ll quantity;
};

//
// io/reader.cpp
//
//
//

namespace io {

class Reader {
  public:
    Reader(VS fileNames) : fileNames(fileNames){
      // TODO: Multiple files support
    };
    Reader(std::string fileName) : fileName(fileName){};
    Reader() = default;

    /**
     * Read file
     */
    std::ifstream gets();

  private:
    VS fileNames;
    std::string fileName;
};

} // namespace io

//
// filter/filter.cpp
//

namespace filter {

class Filter {
  public:
    Filter(std::ifstream infile) {
      this->securities = {};
      this->dws = {}; this->cbbcs = {};

      std::string line;
      while (getline(infile, line)) {
	this->filtering(line);
      }

#ifdef DEBUG
      std::cout << "Filter Init Finished!" << "\n" <<
	"Found " << this->dws.size() << " lines of Derivative Warrants."
	<< "\nFound " << this->cbbcs.size() << " lines of CBBC." << "\n";
#endif

      infile.close();
    };

    void parsing(int type);

  private:
    VS dws;
    VS cbbcs;
    VLLS securities;

  struct mytask {
    mytask(size_t n)
      :_n(n)
    {}
    void operator()() {
    }
    size_t _n;
  };

  template <typename T> struct invoker {
    void operator()(T& it) const {it();}
  };

  protected:
    void pperform(std::string line);
    void filtering(std::string line);
};

} // namespace filter

} // namespace spork
