#pragma once
#include <fstream>
using namespace std;
namespace Wator {
  class Net {
    public: 
      Net();
      Net(const ifstream &in);
  };
}
