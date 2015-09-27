#pragma once
#include <fstream>
#include <memory>
using namespace std;

namespace Wator {
    class Blob;
    class LayerBase {
    public:
        explicit LayerBase();
        explicit LayerBase(const ifstream &in);
    protected:
        shared_ptr<Blob> blob_;
    };
}
