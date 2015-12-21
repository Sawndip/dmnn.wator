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

/**
 * Constructor
 **/
CoulombLayer::CoulombLayer()
{
    for(int y = 0 ;y < h_;y++){
        for(int x =0;x < w_ ;x++){
            // 电荷中心，在几何中心。
            float w = (x- w_/2) * (x- w_/2) + (y- h_/2) * (y- h_/2) +1;
            w = 1/w - 0.25;
            TRACE_VAR(x);
            TRACE_VAR(y);
            weight_.push_back(w);
            INFO_VAR(w);
        }
    }
    INFO_VAR(weight_.size());
}

/**
 * Connect a Layer to Net.
 * @param [in] layer
 **/
V1CortexLayer& CoulombLayer::operator << (V1CortexLayer &layer)
{
    this->top_.push_back(&layer);
    layer.bottom(this);
    INFO_VAR(top_.size());
    return layer;
}


/**
 * update
 * @return None.
 **/
void CoulombLayer::updateW(void){
}


/**
 * update
 * @return None.
 **/
void CoulombLayer::update(void)
{
    INFO_VAR("finnish CoulombLayer::update");
}


/**
 * aspect
 * @return None.
 **/
void CoulombLayer::aspect(int w,int h,int ch)
{
    this->wGrid_ = w;
    this->hGrid_ = h;
    this->chGrid_ = ch;
}


/**
 * round
 * @return None.
 **/
void CoulombLayer::round(void)
{
    this->forward();
    this->update();
    INFO_VAR("finnish CoulombLayer::round");
}


/**
 * forward
 * @return None.
 **/
void CoulombLayer::forward(void)
{
    blobsRaw_.clear();
    blobs_.clear();
    for(auto btm:bottom_){
        LayerInput *input = dynamic_cast<LayerInput*>(btm);
        auto inBlob = input->getBlob(this);
        INFO_VAR(inBlob->size_);
        INFO_VAR(weight_.size());
        size_ = inBlob->size_/weight_.size();
        auto raw = shared_ptr<Blob<float>>(new Blob<float>(size_));
        auto blob = shared_ptr<Blob<bool>>(new Blob<bool>(size_));
        max_ = 0;
        min_ = INT32_MAX;
        for(int i = 0 ;i < size_ ;i++) {
            int sum =0;
            for(int j = 0;j<weight_.size();j++) {
                int index = i*weight_.size() + j;
                sum += weight_[j] * (inBlob->data_[index]);
            }
            if(sum > max_) {
                max_ = sum;
            }
            if(sum < min_) {
                min_ = sum;
            }
            raw->data_[i] = sum;
        }
        blobsRaw_.push_back(raw);
        INFO_VAR(max_);
        INFO_VAR(min_);
        threshold_ = (max_ + min_)/2;
        INFO_VAR(threshold_);
        
        int activeSize = size_;
        while (activeSize > (size_ / activeReciprocal_)) {
            activeSize = 0;
            for(int i = 0 ;i < size_ ;i++) {
                int delta = raw->data_[i] - threshold_;
                if(0 < delta ){
                    blob->data_[i] = true;
                    activeSize++;
                }else{
                    blob->data_[i] = false;
                }
            }
            threshold_ += thresholdStep_;
        }
        INFO_VAR(threshold_);
        INFO_VAR(size_);
        INFO_VAR(activeSize);
        INFO_VAR(size_/activeSize);
        blobs_.push_back(blob);
    }
    INFO_VAR("finnish CoulombLayer::forward");
}
