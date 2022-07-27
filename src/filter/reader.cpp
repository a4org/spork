//===----------------------------------------------------------------------===//
//                         spork
//
// reader
//
// Identification: src/filter/reader.cpp
//
// Last Modified : 2022.7.27 Jiawei Wang
//
// Copyright (c) 2022 Angold-4
//
//===----------------------------------------------------------------------===//

#include "../spork.hpp"

std::ifstream Reader::gets() {
  std::ifstream infile;
  infile.exceptions(std::ifstream::badbit);

  try {
    infile.open(this->fileName);
  } catch (const std::ifstream::failure& e) {
    std::cout << "Exception opening/reading file" << std::endl;
  }

  return infile;
}
