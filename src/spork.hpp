//===----------------------------------------------------------------------===//
//                         spork
//
// spork
//
// Identification: src/filter/spork.hpp
//
// Last Modified : 2022.7.27 Jiawei Wang
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
    Filter(std::ifstream &infile) { // ref
    };

  private:
    VLLS securities;
};

