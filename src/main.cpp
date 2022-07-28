#include "spork.hpp"

int main(int argc, char** argv) {
  std::string filename = argv[1];

  spork::io::Reader* reader = new spork::io::Reader(filename);
  spork::filter::Filter* filter = new spork::filter::Filter(reader->gets());

  filter->parsing(DWT); 
  filter->parsing(CBBCT);

  return 0;
}
