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
PolarizerLayer::PolarizerLayer()
{
    activeWeight_ = {
        2.0,
        2.0,
        2.0,
        2.0,
        16.0,
        2.0,
        2.0,
        2.0,
        2.0,
    };
    deactiveWeight_ = {
        1.0,
        1.0,
        1.0,
        1.0,
        8.0,
        1.0,
        1.0,
        1.0,
        1.0,
    };
    for(int i = 0 ;i< activeWeight_.size();i++) {
        weight_.push_back(activeWeight_[i] - deactiveWeight_[i]);
    }
}

/**
 * round
 * @return None.
 **/
void PolarizerLayer::round(void)
{
    /*
     for(auto btm:bottom_){
     auto blob = btm->getBlob(this);
     }
     */
}

/**
 * forward
 * @return None.
 **/
void PolarizerLayer::forward(void)
{
/*
    for(auto btm:bottom_){
        auto blob = btm->getBlob(this);
    }
 */
}
/**
 * update
 * @return None.
 **/
void PolarizerLayer::update(void)
{
    bandGap_ = UINT32_MAX/2;
    int weight = 0;
    int spitedCounter =0;
    for(auto &raw: blobsRaw_){
        INFO_VAR(raw);
        for(int i = 0 ;i < size_ ;i++) {
            int delta = raw->data_[i] - threshold_;
            unsigned int deltaU = ::abs(delta);
            if(0==delta){
                delta ++;
            }
            weight += UINT16_MAX/delta;
            
            if(deltaU < bandGap_){
                bandGap_ = deltaU;
            }
            if(deltaU > (max_ - min_)/4){
                spitedCounter++;
            }
        }
    }
    INFO_VAR(weight);
    if(0 < weight){
        for(auto &w:activeWeight_){
            w += weight;
        }
    }else {
        for(auto &w:deactiveWeight_){
            w += 0-(weight);
        }
    }
    for(int i = 0 ;i< activeWeight_.size();i++) {
        weight_[i] = activeWeight_[i] - deactiveWeight_[i];
        INFO_VAR(weight_[i]);
    }
    INFO_VAR(bandGap_);
    INFO_VAR((double)spitedCounter/(double)size_);
    INFO_VAR("finnish Polarizer1stLayer::update");
}


/**
 * Constructor
 **/
Polarizer1stLayer::Polarizer1stLayer()
:PolarizerLayer()
{
}

/**
 * round
 * @return None.
 **/
void Polarizer1stLayer::round(void)
{
    this->forward();
    this->update();
    INFO_VAR("finnish Polarizer1stLayer::round");
}


/**
 * forward
 * @return None.
 **/
void Polarizer1stLayer::forward(void)
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
        threshold_ = (max_+min_)/2;
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
    INFO_VAR("finnish Polarizer1stLayer::forward");
}
