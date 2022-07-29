//===----------------------------------------------------------------------===//
//                         spork
//
// bidask
//
// Identification: src/function/bidask
//
// Last Modified : 2022.7.29 Jiawei Wang
//
// Copyright (c) 2022 Angold-4
//
//===----------------------------------------------------------------------===//

#include "../spork.hpp"

namespace spork {
namespace function {

std::vector<Output> BidAskProcessor::start() {
  if (this->securities.size() == 0) {
    std::cerr << "Empty Securities!" << std::endl;
    exit(1);
  }

  int k = 0;

  std::vector<Output> rets = {};
  VSE timelist = {};
  std::string prv = this->securities[0].date; std::string curr;
  ll n = this->securities.size();
  timelist.push_back(this->securities.front());
  security sec;
  for (int i = 1; i < n; i++) {
    sec = this->securities[i];
    curr = sec.date;
    if (prv != curr) {
      perform(timelist, rets, this->mapheaps, this->deletedbid, this->deletedask);
      timelist = {};
      prv = curr;
    }
    timelist.push_back(sec);
  }

  return rets;
}


void BidAskProcessor::perform
(VSE &secs, std::vector<Output> &rets, MPBA &mapheap, MSD &delbid, MSD &delask) {

  Output ret;
  std::string date = secs[0].date;
  std::string code; BA ba;
  std::unordered_set<std::string> uniquetrans = {};


  //
  // First round, update the state just after this moment
  // and record all uniquetrans
  //


  for (security sec : secs) {
    code = sec.code;
    this->sec2ba(ba, sec);
    uniquetrans.insert(code);
    if (sec.ad) { // add
      this->heapInsert(mapheap, ba, sec.bidask);
    } else { // delete
      this->heapDelete(delbid, delask, ba, sec.bidask, mapheap);
    }
  }


  //
  // Second round, for each unique trans, output its state 
  //

  for (std::string code : uniquetrans) {
    double bidp = -1; double askp = -1;
    std::string bidid; std::string askid;
    ll bidq; ll askq;
    // #1 Get the bid price and quantity
    BPQ& bpq = mapheap[code].first;
    this->performUpdate<BPQ>(bpq, bidp, bidq, bidid, delbid);


    // #2 Get the ask price and quantity
    APQ& apq = mapheap[code].second;
    this->performUpdate<APQ>(apq, askp, askq, askid, delask);


    // #3 Assign values to ret
    ret.date = date; ret.code = code;
    ret.askid = askid; ret.askprice = askp;
    ret.bidid = bidid; ret.bidprice = bidp;
    ret.askquantity = askq;
    ret.bidquantity = bidq;

    rets.push_back(ret);
  }
}

void BidAskProcessor::heapInsert(MPBA &mpba, BA ba, bool bidask) { // add
  std::string code = ba.code;
  std::pair<BPQ, APQ>& rheap = mpba[code];
  if (bidask) {
    rheap.first.push(ba);
  } else {
    rheap.second.push(ba);
  }
}

void BidAskProcessor::heapDelete(MSD& msdb, MSD& msda, BA ba, bool bidask, MPBA& mpba) { // delete
  std::string code = ba.code;
  std::string orderId = ba.orderId;
  
  if (bidask) {
    // BPQ& bpq = mpba[code].first;
    msdb.insert(orderId);
    // performDelete<BPQ>(&bpq, code, orderId, msdb);
  } else {
    // APQ& apq = mpba[code].second;
    msda.insert(orderId);
    // performDelete<APQ>(&apq, code, orderId, msda);
  }
}

template<typename C>
void BidAskProcessor::performUpdate(C& abpq, double& bap, ll& baq, std::string& baid, MSD& msdba) {
  if (abpq.size() == 0) {
    bap = 0;
    baq = 0;
    baid = "";
  } else {
    while (!abpq.empty()) {
      BA ba = abpq.top();
      auto it = msdba.find(ba.orderId);
      if (it != msdba.end()) {
	msdba.erase(it);
	abpq.pop();
      } else {
	bap = ba.price;
	baq = ba.quantity;
	baid = ba.orderId;
	break;
      }
    }
    if (bap == -1) {
      bap = 0; 
      baq = 0;
      baid = "";
    }
  }
}

template<typename C>
void BidAskProcessor::performDelete(C* abpq, std::string code, std::string orderId, MSD& msdba) {
  if (orderId == abpq->top().orderId) {
    // fast way, don't bother record it, just delete it
    abpq->pop();
  } else {
    // record it, hash every time
    msdba.insert(orderId);
  }
}

} // namespace function
} // namespace spork
