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

void Filter::filtering(std::string line) {
  if (line[17] != ',') {
    std::cerr << "Invalid date/record" << '\n';
    return;
  }

  int i = 18; 
  std::string securityCode = "";
  while (line[i] != ',') {
    securityCode += line[i];
  }

  int securityNum = std::stoi(securityCode);

  if (securityNum >= DWLB && securityNum <= DWUB) {
    this->dws.push_back(line);
  } else if (securityNum >= CBBCLB && securityNum <= CBBCUB) {
    this->cbbcs.push_back(line);
  }
}

