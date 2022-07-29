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
    if (this->idata.size()) {
      for (auto ps : this->idata) {
	ofile << this->sec2csv(ps.second);
      }
    } else if (this->out.size()) {
      for (auto o : this->out) {
	ofile << this->out2csv(o);
      }
    }
  }

  std::string Writer::out2csv(Output& o) {
    std::string ret = "";
    ret += o.date; ret += ',';
    ret += o.code; ret += ',';
    ret += o.bidid; ret += ',';
    ret += o.askid; ret += ',';
    ret += std::to_string(o.bidprice); ret += ',';
    ret += std::to_string(o.bidquantity); ret += ',';
    ret += std::to_string(o.askprice); ret += ',';
    ret += std::to_string(o.askquantity); ret += ',';

    ret += '\n';

    return ret;
  }

  std::string Writer::sec2csv(security& s) {
    std::string ret = "";

    ret += s.date; ret += ','; ret += s.code; ret += ',';
    ret += (s.ad == true) ? "A" : "D"; ret += ',';
    ret += s.orderId; ret += ',';
    ret += (s.bidask == true) ? "0" : "1"; ret += ',';
    ret += (s.price == (double) -1) ? "" : std::to_string(s.price); ret += ',';
    ret += (s.quantity == (ll) -1) ? "" : std::to_string(s.quantity); ret += ',';

    ret += '\n';

    return ret;
  }
} // namespace io
} // namespace spork
