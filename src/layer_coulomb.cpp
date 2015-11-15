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
    updateW();
}

/**
 * update
 * @return None.
 **/
void CoulombLayer::updateW(void)
{
    activeWeight_.clear();
    deactiveWeight_.clear();
    weight_.clear();
    for(int y = 0 ;y < h_;y++){
        for(int x =0;x < w_ ;x++){
            float w = (x- w_/2) * (x- w_/2) + (y- h_/2) * (y- h_/2) + 1;
            w = (1/w);
            activeWeight_.push_back(w * activeRate);
            
            
            deactiveWeight_.push_back(0 - w * deactiveRate);

            weight_.push_back(w*activeRate - w * deactiveRate);
        }
    }
    for(int i = 0 ;i< activeWeight_.size();i++) {
        INFO_VAR(weight_[i]);
    }
}


/**
 * update
 * @return None.
 **/
void CoulombLayer::update(void)
{
    bandGap_ = UINT32_MAX/2;
    int spitedCounter =0;
    for(auto &raw: blobsRaw_){
        INFO_VAR(raw);
        for(int i = 0 ;i < size_ ;i++) {
            int delta = raw->data_[i] - threshold_;
            unsigned int deltaU = ::abs(delta);
            if(0==delta){
                delta ++;
            }
            auto rate = learnRate* delta/allGap_;
            if(rate>0){
                activeRate += rate;
            } else {
                deactiveRate -= rate;
            }
            
            if(deltaU < bandGap_){
                bandGap_ = deltaU;
            }
            if(deltaU > (allGap_)/4){
                spitedCounter++;
            }
        }
    }
    updateW();
    INFO_VAR(bandGap_);
    INFO_VAR((double)spitedCounter/(double)size_);
    INFO_VAR("finnish CoulombLayer::update");
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
        auto input = dynamic_pointer_cast<LayerInput>(btm);
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
        allGap_ = max_ - min_;
        INFO_VAR(threshold_);
        for(int i = 0 ;i < size_ ;i++) {
            int delta = raw->data_[i] - threshold_;
            if(0 < delta ){
                blob->data_[i] = true;
            }else{
                blob->data_[i] = false;
            }
        }
        blobs_.push_back(blob);
    }
    INFO_VAR("finnish CoulombLayer::forward");
}
