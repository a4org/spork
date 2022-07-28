#include "spork.hpp"

int main(int argc, char** argv) {
  std::string filename = argv[1];

  spork::Reader* reader = new spork::Reader(filename);
  spork::Filter* filter = new spork::Filter(reader->gets());

  filter->parsing(DWT); 
  filter->parsing(CBBCT);

  return 0;
}
