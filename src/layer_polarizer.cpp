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
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/2,
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/(w_*h_*4),
        UINT16_MAX/(w_*h_*4)
    };
    deactiveWeight_ = {
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(2*2),
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(w_*h_*8),
        UINT16_MAX/(w_*h_*8)
    };
    for(int i = 0 ;i< activeWeight_.size();i++) {
        weight_.push_back(activeWeight_[i] - deactiveWeight_[i]);
    }
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
 * Constructor
 **/
Polarizer1stLayer::Polarizer1stLayer()
:PolarizerLayer()
{
}

/**
 * forward
 * @return None.
 **/
void Polarizer1stLayer::forward(void)
{
     for(auto btm:bottom_){
         auto input = dynamic_pointer_cast<LayerInput>(btm);
         auto inBlob = input->getBlob(this);
         INFO_VAR(inBlob->size_);
         INFO_VAR(weight_.size());
         const auto mySize = inBlob->size_/weight_.size();
         auto raw = shared_ptr<Blob<int>>(new Blob<int>(mySize));
         auto blob = shared_ptr<Blob<bool>>(new Blob<bool>(mySize));
         int max = 0;
         int min = INT32_MAX;
         for(int i = 0 ;i < mySize ;i++) {
             int sum =0;
             for(int j = 0;j<weight_.size();j++) {
                 int index = i*weight_.size() + j;
                 if(0==inBlob->data_[index]){
                     INFO_VAR(index);
                 }
                 sum += weight_[j] * (inBlob->data_[index]);
             }
             if(sum > max) {
                 max = sum;
             }
             if(sum < min) {
                 min = sum;
             }
             raw->data_[i] = sum;
         }
         blobsRaw_.push_back(raw);
         INFO_VAR(max);
         INFO_VAR(min);
         auto threshold_ = (max+min)/2;
         INFO_VAR(threshold_);
         for(int i = 0 ;i < mySize ;i++) {
            int delta = raw->data_[i] - threshold_;
            const unsigned int deltaU = ::abs(delta);
             if(deltaU < bandgap_) {
                 bandgap_ = deltaU;
             }
             if(0 < delta ){
                 blob->data_[i] = true;
             }else{
                 blob->data_[i] = false;
             }
         }
         INFO_VAR(bandgap_);
         blobs_.push_back(blob);
     }
    INFO_VAR("finnish Polarizer1stLayer::forward");
}

