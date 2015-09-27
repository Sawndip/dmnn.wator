#pragma once
#include <fstream>
#include <memory>
#include <vector>
using namespace std;
namespace Wator {
    class LayerBase;
    class Net {
    public:
        explicit Net();
        explicit Net(const ifstream &in);
    private:
        vector<shared_ptr<LayerBase> > layers_;
    };
}
