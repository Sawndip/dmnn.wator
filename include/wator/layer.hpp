#pragma once
#include <fstream>
using namespace std;
namespace Wator {
  class LayerBase {
    public: 
      LayerBase();
      LayerBase(const ifstream &in);
  };
}
