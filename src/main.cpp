#include "spork.hpp"

int main(int argc, char** argv) {
  std::string filename = argv[1];
  std::string outname1 = argv[2];
  std::string outname2 = argv[3];

  spork::io::Reader* reader = new spork::io::Reader(filename);
  spork::filter::Filter* filter = new spork::filter::Filter(reader->gets());

  filter->deletect = 0;

  // filter->parsing(DWT); 
  filter->parsing(CBBCT);

  std::cout << filter->deletect << std::endl;

  spork::io::Writer* writer = new spork::io::Writer(filter->getOutput());
  writer->wSingleCSV(outname1);

  std::vector<spork::security> securities;

  int ct = 0;
  for (auto p : filter->getOutput()) {
    // std::cout << p.second.code << std::endl;
    securities.push_back(p.second);
  }
  std::cout << ct << std::endl;

  spork::function::BidAskProcessor* BAP = new spork::function::BidAskProcessor(securities);
  
#ifdef DEBUG
  std::cout << "BidAsk Processing ..." << '\n';
#endif

  std::vector<spork::Output> out2 =  BAP->start();
  spork::io::Writer* writer2 = new spork::io::Writer(out2);
  writer2->wSingleCSV(outname2);

  return 0;
}
