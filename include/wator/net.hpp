#pragma once
#include <ifstream>
using namespace std;
namespace Wator {
  class Net {
    public: 
      Net();
      Net(const ifstream &in);
  }
}
