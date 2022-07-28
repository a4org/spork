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

#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/task_scheduler_init.h"


#define DWLB 10000
#define DWUB 29999
#define CBBCLB 50000
#define CBBCUB 69999
#define VS std::vector<std::string>
#define VLLS std::vector<std::pair<long long, security>> 

using ll = long long;

struct security {
  std:: string date;
  std::string code;
  bool ad; // T: add, F: delete
  ll orderId;
  bool bidask; // T: bid, F: ask
  double price;
  ll quantity;
};

//
// filter/reader.cpp
//
//

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

//
// filter/filter.cpp
//

class Filter {
  public:
    Filter(std::ifstream &infile, VS types) { // ref
      this->securities = {};
      this->dws = {}; this->cbbcs = {};
      this->types = types;

      std::string line;
      while (getline(infile, line)) {
	filtering(line);
      }
      infile.close();
    };

  private:
    VS types;
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
    void filtering(std::string line);
    void parsingLine(std::string line);
};

