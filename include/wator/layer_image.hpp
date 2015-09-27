#pragma once
#include <fstream>
#include <memory>
using namespace std;
#include "wator/layer.hpp"
#include <opencv2/core/core.hpp>

namespace Wator {
    class ImageLayer :public LayerBase {
    public:
        explicit ImageLayer();
    private:
        cv::Mat data_;
    };
}
