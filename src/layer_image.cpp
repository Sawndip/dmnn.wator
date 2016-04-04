/*
Copyright (c) 2015, Wator Vapor
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of wator nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "wator.hpp"
using namespace Wator;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;



#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#include <algorithm>
using namespace std;

/**
 * Constructor
 **/
ImageLayer::ImageLayer()
:extension_({"jpg",".JPG",".png",".PNG",".tif",".TIF"})
{
    INFO_VAR(this);
}

/**
 * Connect a Layer to Net.
 * @param [in] layer
 **/
CoulombLayer& ImageLayer::operator << (CoulombLayer &layer)
{
    this->top_.push_back(&layer);
    layer.bottom(this);
    INFO_VAR(top_.size());
    return layer;
}

/**
 * load
 * @return None.
 **/
void ImageLayer::load(bool train)
{
    TRACE_VAR(param_.root_);
    const fs::path path(param_.root_);
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
        if (!fs::is_directory(p)){
            auto extension = p.extension().string();
            auto itExt = std::find(extension_.begin(),extension_.end(),extension);
            if(itExt != extension_.end()){
                INFO_VAR(p);
                mat_ = cv::imread(p.string());
                pump();
                for(auto &top:top_)
                {
                    if (train) {
                        top->round();
                    } else {
                        top->forward();
                    }
                }
            }
        }
    }
    INFO_VAR("finnish load data");
}

/*
 x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12
 y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12
 z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12
 u1,u2,u3,u4,u5,u6,u7,u8,u9,u10,u11,u12
 v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12
 w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,w11,w12
 
 ------------------
 x1,x2,x3,y1,y2,y3,z1,z2,z3,  
 x4,x5,x6,y4,y5,y6,z4,z5,z6, 
 x7,x8,x9,y7,y8,y9,z7,z8,z9,
 x10,x11,x12,y10,y11,y12,z10,z11,z12,
 u1,u2,u3,v1,v2,v3,w1,w2,w3,
 u4,u5,u6,v4,v5,v6,w4,w5,w6,
 u7,u8,u9,v7,v8,v9,w7,w8,w9,
 u10,u11,u12,v10,v11,v12,w10,w11,w12
 */

void ImageLayer::pump(void)
{
    blobs_.clear();
    for(int i = 0;i < top_.size();i++ )
    {
        INFO_VAR(mat_.cols);
        INFO_VAR(mat_.rows);
        INFO_VAR(mat_.channels());
        auto blob = new Blob<uint8_t>(mat_.cols,mat_.rows,mat_.channels());
        INFO_VAR(this);
        blobs_.push_back(blob);
    }
    std::vector<cv::Mat> planes;
    cv::split(mat_, planes);
    for(int channel = 0 ; channel < mat_.channels();channel++){
        auto &mat = planes[channel];
        for(int y = 0;y < mat.rows;y++){
            for(int x = 0;x < mat.cols;x++){
                auto byte = mat.at<uint8_t>(y, x);
                TRACE_VAR(x);
                TRACE_VAR(y);
                for(int i = 0;i < top_.size();i++){
                    CoulombLayer *clm = dynamic_cast<CoulombLayer*>(top_[i]);
                    //clm->aspect(mat.cols/clm->w_,mat.rows/clm->h_,mat_.channels());
                    TRACE_VAR(clm->w_);
                    TRACE_VAR(clm->h_);
                    int grid = ((y/clm->h_) * (mat_.cols/clm->w_))+ (x/clm->w_) ;
                    TRACE_VAR(grid);
                    TRACE_VAR(channel);
                    
                    int index = channel * mat_.cols * mat_.rows;
                    index += grid * clm->w_ * clm->h_;
                    index += (y%clm->h_)*clm->w_  + x%clm->w_ ;
                    TRACE_VAR(index);
                    blobs_[i]->data_[index] = byte;
                }
            }
        }
    }
    INFO_VAR(mat_.cols*mat_.rows*mat_.channels());
}
