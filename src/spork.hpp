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
#include <unordered_map>
#include <unordered_set>

#define DEBUG 1
#define DWT 1
#define DWLB 10000
#define DWUB 29999
#define CBBCT 2
#define CBBCLB 50000
#define CBBCUB 69999
#define VI std::vector<int>
#define VS std::vector<std::string>
#define VSE std::vector<security>
#define VLLS std::vector<std::pair<long long, security>> 
#define BPQ std::priority_queue<BA, std::vector<BA>, bidcmp>
#define APQ std::priority_queue<BA, std::vector<BA>, askcmp>
#define MPBA std::unordered_map<std::string, std::pair<BPQ, APQ>>
#define MSD std::unordered_set<std::string>

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

struct Output {
  std::string date;
  std::string code;
  std::string askid;
  std::string bidid;
  double askprice;
  double bidprice;
  ll askquantity;
  ll bidquantity;
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
    Writer(VLLS data) : idata(data) { this->out = {}; };
    Writer(std::vector<spork::Output> out) : out(out) { this->idata = {}; };

    /**
     * Write to a single CSV file
     */
    void wSingleCSV(std::string wfilename);

  protected:
    std::string sec2csv(security& s);
    std::string out2csv(Output& s);
  private:
    VLLS idata;
    std::vector<spork::Output> out;  
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

struct bidcmp {
  bool operator()(BA const& leftb, BA const& rightb) {
    return leftb.price > rightb.price;
  }
};

struct askcmp {
  bool operator()(BA const& lefta, BA const& righta) {
    return lefta.price < righta.price;
  }
};


class BidAskProcessor {
  public:
    BidAskProcessor(VSE securities) {
      this->securities = securities;
      this->mapheaps = {};
      this->deletedbid = {};
      this->deletedask = {};
    }
    std::vector<Output> start();

    void test() {
      // for debug
      BA debugba;
      BPQ debugbpq = {};
      for (auto s : this->securities) {
	sec2ba(debugba, s);
	debugbpq.push(debugba);
      }

    }

  protected:
    void perform(VSE& secs, std::vector<Output>& ret, MPBA& mpba, MSD& db, MSD& da);

  private:
    std::unordered_map<std::string, std::pair<BPQ, APQ>> mapheaps;
    std::unordered_set<std::string> deletedask;
    std::unordered_set<std::string> deletedbid;
    VSE securities;

    void sec2ba(BA &ba, security &se) {
      ba.price = se.price;
      ba.code = se.code;
      ba.ad = se.ad; ba.orderId = se.orderId; ba.quantity = se.quantity;
    }

    std::string getTime(security* sec) {
      return sec->date;
    }

    template<typename C>
    void performUpdate(C& abpq, double& bap, ll& baq, std::string& baid, MSD& msdba);

    template<typename C>
    void performDelete(C& abpq, std::string code, std::string orderId, MSD& msdba);

    void heapInsert(MPBA& mpba, BA ba, bool bidask);
    void heapDelete(MSD& msdb, MSD& msda, BA ba, bool bidask, MPBA& mpba);
};

} // namespace function

} // namespace spork
