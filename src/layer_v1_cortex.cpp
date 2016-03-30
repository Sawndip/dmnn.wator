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
    memory_ = new ImplicitMemory;
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
    blobs_.clear();
    for(auto btm:bottom_){
        CoulombLayer *coulom  = dynamic_cast<CoulombLayer*>(btm);
        auto coulomBlob = coulom->getBlob(this);
        INFO_VAR(coulomBlob->w_);
        INFO_VAR(coulomBlob->h_);
        INFO_VAR(coulomBlob->ch_);
//        const int size = (coulomBlob->w_/this->w_ )* (coulomBlob->h_ /this->h_)* coulomBlob->ch_;
        for (auto top:top_) {
            auto pinch = new Blob<bool>(coulomBlob->w_,coulomBlob->h_,coulomBlob->ch_);
            for (int ch = 0; ch < coulomBlob->ch_; ch++) {
                for (int y = 0; y < coulomBlob->h_; y++) {
                    for (int x = 0; x < coulomBlob->w_; x++) {
                        int grid = (y/this->h_) * (coulomBlob->w_/this->w_) + (x/this->w_) ;
                        int index = ch * coulomBlob->w_ * coulomBlob->h_;
                        index += grid * this->w_ * this->h_;
                        index += (y%this->h_)*this->w_  + x%this->w_ ;
                        TRACE_VAR(index);
                        TRACE_VAR(x);
                        TRACE_VAR(y);
                        int index2 = ch * coulomBlob->w_ * coulomBlob->h_;
                        index2 += y*coulomBlob->w_ + x;
                        TRACE_VAR(index2);
                        if (index >= coulomBlob->size_|| index2 >= coulomBlob->size_) {
                            // 无法整除的最后几行，不能的到下一层的完整输出，省略。
                            TRACE_VAR(this->w_);
                            TRACE_VAR(this->h_);
                            TRACE_VAR(grid);
                            TRACE_VAR(x);
                            TRACE_VAR(y);
                            TRACE_VAR(index);
                            TRACE_VAR(index2);
                            TRACE_VAR(coulomBlob->w_);
                            TRACE_VAR(coulomBlob->h_);
                            TRACE_VAR(coulomBlob->size_);
                            continue;
                        }
                        pinch->data_[index] = coulomBlob->data_[index2];
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
            memory_->update(index,memIndex,this->sparse_);
        }
    }
    memory_->sort();
#if 0
    for(int index = 0; index < pinchs_.size();index++) {
        auto &pinch = pinchs_[index];
        for (int i =0; i < pinch->size_; i + = this->w_*this->h_) {
            for (int j = 0; j < this->w_*this->h_; j++) {
                ;
            }
        }
    }
#endif
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
