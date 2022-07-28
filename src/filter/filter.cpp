//===----------------------------------------------------------------------===//
//                         spork
//
// filter
//
// Identification: src/filter/filter.cpp
//
// Last Modified : 2022.7.28 Jiawei Wang
//
// Copyright (c) 2022 Angold-4
//
//===----------------------------------------------------------------------===//

#include "../spork.hpp"

namespace spork {
namespace filter {

void Filter::filtering(std::string line) {

  if (line[17] != ',') {
    std::cerr << "Invalid date/record." << '\n';
    exit(1); // invalid input, exit
  }

  int i = 18; 
  std::string securityCode = "";
  while (line[i] != ',') {
    securityCode += line[i];
    i++;
  }

  int securityNum = std::stoi(securityCode);

  if (securityNum >= DWLB && securityNum <= DWUB) {
    this->dws.push_back(line);
  } else if (securityNum >= CBBCLB && securityNum <= CBBCUB) {
    this->cbbcs.push_back(line);
  }
}

void Filter::parsing(int type) {
  switch (type) {
    case DWT:
      for (std::string dw : this->dws) {
	this->pperform(dw);
      }
      break;
    case CBBCT:
      for (std::string cbbc : this->cbbcs) {
	this->pperform(cbbc);
      }
      break;
  }
}

// execution unit 20140401094559859,64209,A,2582549,1,0.157,200000, void Filter::pperform(std::string line) {
void Filter::pperform(std::string line) {
  security s;

  const int n = line.size();
  std::string tmp = ""; int i = 0; int p = 0;
  std::string tmpstored[7];

  while (i < n && p != 7) {
    while (line[i] != ',') {
      tmp.push_back(line[i]);
      i++;
    }
    tmpstored[p] = tmp;
    tmp = "";
    p++; i++;
  }

  if (p != 7) {
    std::cerr << "Invalid record format: p = " << p << '\n';
    exit(1); // invalid input, exit
  }

  s.date = tmpstored[0];
  s.code = tmpstored[1];
  s.ad = (tmpstored[2] == "A") ? true : false;
  s.orderId = tmpstored[3];
  s.bidask = (tmpstored[4] == "0") ? true : false;
  s.price = tmpstored[5] == "" ? -1 : std::stod(tmpstored[5]);
  s.quantity = tmpstored[6] == "" ? -1 : std::stoll(tmpstored[6]);

#ifdef DEBUG
  std::cout << "date: " << s.date << ", code: " << s.code << ", ad: " << s.ad
    << ", orderId: " << s.orderId << ", bidask: " <<  s.bidask
    << ", price: " << s.price << ", quantity: " << s.quantity << '\n';
#endif

  int daten = std::stoll(s.date);

  this->securities.push_back({daten, s});
}

} // namespace filter
} // namespace spork
