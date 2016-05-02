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

#include <bitset>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;

/**
 * Constructor
 **/
V1CortexLayer::V1CortexLayer()
{
}
V1CortexLayer::~V1CortexLayer()
{
    INFO_VAR(this);
}




/**
 * update
 * @return None.
 **/
void V1CortexLayer::update(void)
{
    INFO_VAR("finnish V1CortexLayer::update");
}




/**
 * round
 * @return None.
 **/
void V1CortexLayer::round(void)
{
    this->forward();
    this->update();
    INFO_VAR("finnish V1CortexLayer::round");
    LayerBase::round();
    this->dump();
}


/**
 * forward
 * @return None.
 **/
void V1CortexLayer::forward(void)
{
    pinchs_.clear();
    raws_.clear();
    blobs_.clear();
    for(auto btm:bottom_){
        shared_ptr<Blob<bool>> inputBlob;
        auto coulomb  = dynamic_pointer_cast<CoulombLayer>(btm);
        if(coulomb) {
            inputBlob = coulomb->getBlob(this);
        }
        auto einstein  = dynamic_pointer_cast<EinsteinLayer>(btm);
        if(einstein) {
            inputBlob = einstein->getBlob(this);
        }
        if(nullptr == inputBlob) {
            continue;
        }
        INFO_VAR(inputBlob->w_);
        INFO_VAR(inputBlob->h_);
        INFO_VAR(inputBlob->ch_);
//        const int size = (inputBlob->w_/this->w_ )* (inputBlob->h_ /this->h_)* inputBlob->ch_;
        for (auto top:top_) {
            auto pinch = new Blob<bool>(inputBlob->w_,inputBlob->h_,inputBlob->ch_);
            for (int ch = 0; ch < inputBlob->ch_; ch++) {
                for (int y = 0; y < inputBlob->h_; y++) {
                    for (int x = 0; x < inputBlob->w_; x++) {
                        int grid = (y/this->h_) * (inputBlob->w_/this->w_) + (x/this->w_) ;
                        int index = ch * inputBlob->w_ * inputBlob->h_;
                        index += grid * this->w_ * this->h_;
                        index += (y%this->h_)*this->w_  + x%this->w_ ;
                        TRACE_VAR(index);
                        TRACE_VAR(x);
                        TRACE_VAR(y);
                        int index2 = ch * inputBlob->w_ * inputBlob->h_;
                        index2 += y*inputBlob->w_ + x;
                        TRACE_VAR(index2);
                        if (index >= inputBlob->size_|| index2 >= inputBlob->size_) {
                            // 无法整除的最后几行，不能的到下一层的完整输出，省略。
                            TRACE_VAR(this->w_);
                            TRACE_VAR(this->h_);
                            TRACE_VAR(grid);
                            TRACE_VAR(x);
                            TRACE_VAR(y);
                            TRACE_VAR(index);
                            TRACE_VAR(index2);
                            TRACE_VAR(inputBlob->w_);
                            TRACE_VAR(inputBlob->h_);
                            TRACE_VAR(inputBlob->size_);
                            continue;
                        }
                        pinch->data_[index] = inputBlob->data_[index2];
                    }
                }
            }
            pinchs_.push_back(pinch);
        }
    }
    INFO_VAR(pinchs_.size());
    for(int index = 0; index < pinchs_.size();index++) {
        auto pinch = pinchs_[index];
        TRACE_VAR(pinch->size_);
        for (int i = 0;i < pinch->size_;i += this->w_*this->h_) {
            uint64_t memIndex = 0;
            for (int j = 0; j < this->w_*this->h_; j++) {
                auto index = i + j;
                // 无法整除的最后几点，不能的到下一层的完整输出，省略。
                if (index >= pinch->size_) {
                    continue;
                }
                memIndex = memIndex <<1;
                if (pinch->data_[index]) {
                    memIndex++;
                }
            }
            memory_->update(index,memIndex,this->sparse_,this->w_,this->h_);
        }
    }
    memory_->sort();
#if 1
    for(int index = 0; index < pinchs_.size();index++) {
        auto &pinch = pinchs_[index];
        auto raw = new Blob<int>(pinch->w_/this->w_,pinch->h_/this->h_,pinch->ch_);
        int rawIndex = 0;
        for (int i =0; i < pinch->size_; i += this->w_*this->h_) {
            memory_->clearSearchOnce();
            auto line = memory_->getNext(index);
            while(0 != line) {
                std::bitset<25> memBit(line);
//                INFO_VAR(memBit);
                auto sum = 0;
                for (int j = 0; j < this->w_*this->h_; j++) {
                    int index = i+j;
                    if(pinch->data_[index] && memBit[j]) {
                        sum++;
                    }
                }
                TRACE_VAR(memBit);
                TRACE_VAR(sum);
                TRACE_VAR(raw->size_);
                TRACE_VAR(rawIndex);
                if(rawIndex >= raw->size_) {
                    break;
                }
                if(sum > raw->data_[rawIndex]) {
                    raw->data_[rawIndex] = sum;
                }
                line = memory_->getNext(index);
            }
            rawIndex++;
        }
        raws_.push_back(raw);
    }
#endif
    for(int index = 0; index < raws_.size();index++) {
        auto &raw = raws_[index];
        auto blob = new Blob<bool>(raw->w_,raw->h_,raw->ch_);
        int rawIndex = 0;
        for (int i =0; i < raw->size_; i++) {
            if(raw->data_[i] >= this->sparse_) {
                blob->data_[i] = true;
            } else {
                blob->data_[i] = false;
            }
        }
        blobs_.push_back(blob);
    }
    INFO_VAR(blobs_.size());
    INFO_VAR("finnish V1CortexLayer::forward");
}

/*
 528
 0001000010000100001000010
 
 
 480
 00010
 00010
 00010
 00010
 00010
 
 */


/**
 * dump to png
 * @return None.
 **/
void V1CortexLayer::dump(void){
    INFO_VAR(blobs_.size());
    for (auto blob:blobs_) {
        blob->dump(typeid(this).name());
    }
}
