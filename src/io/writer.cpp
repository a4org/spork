//===----------------------------------------------------------------------===//
//                         spork
//
// writer
//
// Identification: src/io/writer.cpp
//
// Last Modified : 2022.7.28 Jiawei Wang
//
// Copyright (c) 2022 Angold-4
//
//===----------------------------------------------------------------------===//

#include "../spork.hpp"

namespace spork {
namespace io {
  void Writer::wSingleCSV(std::string filename) {
    std::ofstream ofile;
    ofile.open(filename, std::ios_base::app); // append
    for (auto ps : this->idata) {
      ofile << this->csvline(ps.second);
    }
  }

  std::string Writer::csvline(security s) {
    std::string ret = "";

    ret += s.date; ret += ',';
    ret += s.code; ret += ',';
    ret += (s.ad == true) ? "A" : "D"; ret += ',';
    ret += s.code; ret += ',';
    ret += (s.bidask == true) ? "0" : "1"; ret += ',';
    ret += (s.price == (double) -1) ? "" : std::to_string(s.price); ret += ',';
    ret += (s.quantity == (ll) -1) ? "" : std::to_string(s.quantity); ret += ',';

    ret += '\n';

    return ret;
  }
} // namespace io
} // namespace spork
