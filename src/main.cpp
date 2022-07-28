#include "spork.hpp"

int main(int argc, char** argv) {
  std::string filename = argv[1];
  std::string outname = argv[2];

  spork::io::Reader* reader = new spork::io::Reader(filename);
  spork::filter::Filter* filter = new spork::filter::Filter(reader->gets());

  filter->parsing(DWT); 
  filter->parsing(CBBCT);

  spork::io::Writer* writer = new spork::io::Writer(filter->getOutput());
  writer->wSingleCSV(outname);
  return 0;
}
