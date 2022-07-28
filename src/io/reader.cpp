//===----------------------------------------------------------------------===//
//                         spork
//
// reader
//
// Identification: src/io/reader.cpp
//
// Last Modified : 2022.7.28 Jiawei Wang
//
// Copyright (c) 2022 Angold-4
//
//===----------------------------------------------------------------------===//

#include "../spork.hpp"

namespace spork {
namespace io {

std::ifstream Reader::gets() {
  std::ifstream infile;
  infile.exceptions(std::ifstream::badbit);

  try {
    infile.open(this->fileName);
  } catch (const std::ifstream::failure& e) {
    std::cerr << "Exception opening/reading file" << std::endl;
  }

#ifdef DEBUG
  int ct = 0; std::string line;
  while (getline(infile, line)) ct++;
  std::cout << "Reader finished! Found " << ct << " lines of data"<< '\n';
  infile.clear();
  infile.seekg(0);
#endif

  return infile;
}

} // namespace io
} // namespace spork
