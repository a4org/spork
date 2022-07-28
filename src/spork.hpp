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

#include <set>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>

#define DEBUG
#define DWT 1
#define DWLB 10000
#define DWUB 29999
#define CBBCT 2
#define CBBCLB 50000
#define CBBCUB 69999
#define VI std::vector<int>
#define VS std::vector<std::string>
#define VSE std::vector<security*>
#define VLLS std::vector<std::pair<long long, security>> 
#define BPQ std::priority_queue<BA*, std::vector<BA*>, decltype(&bidcmp)>
#define APQ std::priority_queue<BA*, std::vector<BA*>, decltype(&askcmp)>

using ll = long long;

namespace spork {

struct security {
  std::string date;
  std::string code;
  bool ad; // T: add, F: delete
  std::string orderId;
  bool bidask; // T: bid, F: ask
  double price;
  ll quantity;
};


namespace io {

//
// io/reader.cpp
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
// io/writer.cpp
//

class Writer {
  public:
    Writer(VLLS data) : idata(data) {};

    /**
     * Write to a single CSV file
     */
    void wSingleCSV(std::string wfilename);
  protected:
    std::string csvline(security s);

  private:
    VLLS idata;
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

    int deletect;

    void parsing(int type);

    VLLS getOutput() {
      return this->securities;
    }

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

namespace function {

// highest bid, lowest ask
struct BA {
  std::string code;
  bool ad; // T: add, F: delete
  std::string orderId;
  double price;
  ll quantity;
};

static bool bidcmp(BA* leftb, BA* rightb) { return leftb->price > rightb->price; }
static bool askcmp(BA* lefta, BA* righta) { return lefta->price < righta->price; }


class BidAskProcessor {
  public:
    BidAskProcessor(VSE securities) {
      this->securities = securities;
      this->bidheap = {}; this->askheap = {};
      this->preprocessing(this->bidheap, this->askheap);
    }

  protected:
    void preprocessing(BPQ bpq, APQ apq) {
      for (security* sp : securities) {
	BA ba;
	if (sp->bidask == true) {
	  this->setval(&ba, sp);
	  bpq.push(&ba);
	} else {
	  this->setval(&ba, sp);
	  apq.push(&ba);
	}
      }
    }

  private:
    BPQ bidheap;
    BPQ askheap;
    VSE securities;
    std::set<std::string> deleted; // all deleted securities' code

    void setval(BA* ba, security* se) {
      ba->price = se->price;
      ba->code = se->code;
      ba->ad = se->ad;
      ba->orderId = se->orderId;
      ba->quantity = se->quantity;
    }
};

} // namespace function

} // namespace spork
