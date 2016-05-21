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
LeibnizLayer::LeibnizLayer()
{
}
LeibnizLayer::~LeibnizLayer()
{
    INFO_VAR(this);
}



/**
 * update
 * @return None.
 **/
void LeibnizLayer::update(void)
{
    INFO_VAR("finnish LeibnizLayer::update");
}




/**
 * round
 * @return None.
 **/
void LeibnizLayer::round(void)
{
    this->forward();
    this->update();
    INFO_VAR("finnish LeibnizLayer::round");
    LayerBase::round();
    this->dump();
}


/**
 * cal 4 vecotor.
 * @return None.
 **/
void LeibnizLayer::cal4Vec(uint8_t *start,uint8_t &maxDiff)
{
    uint8_t diff1 = std::abs(start[0] - start[1]);
    uint8_t diff2 = std::abs(start[0] - start[2]);
    uint8_t diff3 = std::abs(start[0] - start[3]);
    uint8_t diff4 = std::abs(start[1] - start[2]);
    uint8_t diff5 = std::abs(start[1] - start[3]);
    uint8_t diff6 = std::abs(start[2] - start[3]);
    
    maxDiff = diff1;
    if(diff2 > maxDiff) {
        maxDiff = diff2;
    }
    if(diff3 > maxDiff) {
        maxDiff = diff3;
    }
    if(diff4 > maxDiff) {
        maxDiff = diff4;
    }
    if(diff5 > maxDiff) {
        maxDiff = diff5;
    }
    if(diff6 > maxDiff) {
        maxDiff = diff6;
    }
}


/**
 * forward
 * @return None.
 **/
void LeibnizLayer::forward(void)
{
    pinchs_.clear();
    raws_.clear();
    blobs_.clear();
    for(auto btm:bottom_){
        shared_ptr<Blob<bool>> inputBlob;
        auto newton  = dynamic_pointer_cast<NewtonLayer>(btm);
        if(newton) {
            inputBlob = newton->getBlob2X2(this);
        }
        if(nullptr == inputBlob) {
            continue;
        }
        INFO_VAR(inputBlob->w_);
        INFO_VAR(inputBlob->h_);
        INFO_VAR(inputBlob->ch_);
        for (auto top:top_) {
            auto pinch = inputBlob->grid(0,0,this->w_,this->h_);
            pinchs_.push_back(pinch);
        }
    }
    INFO_VAR(pinchs_.size());
    for(int index = 0; index < pinchs_.size();index++) {
        auto pinch = pinchs_[index];
        TRACE_VAR(pinch->size_);
        auto blobRaw = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(pinch->w_/this->w_,pinch->h_/this->h_,pinch->ch_));
        int blobIndex = 0;
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
            if(blobIndex++ >= blobRaw->size_) {
                continue;
            }
            std::bitset<9> memBit(memIndex);
            blobRaw->data_[blobIndex] = memBit.count();
        }
        // 2x2 max diff
        auto blobRawGrid = blobRaw->grid(0,0,2,2);
        uint8_t max = 0;
        uint8_t min = 255;
        auto blobW = blobRawGrid->w_/2;
        auto blobH = blobRawGrid->h_/2;
        auto raw = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(blobW,blobH,blobRawGrid->ch_));

        for(int i = 0 ;i < raw->size_ ;i++) {
            uint8_t maxDiff = 0;
            uint8_t avg = 0;
            int index = i*2*2;
            cal4Vec(&(blobRawGrid->data_[index]),maxDiff);
            raw->data_[i] = maxDiff;
            
            if(raw->data_[i] > max) {
                max = raw->data_[i];
            }
            if(raw->data_[i] < min) {
                min = raw->data_[i];
            }
        }
        auto blob = shared_ptr<Blob<bool>>(new Blob<bool>(blobW,blobH,blobRawGrid->ch_));
        for (int ch = 0; ch < blobRawGrid->ch_; ch++) {
            const int oneChannel = blobW * blobH;
            const int maxActive = oneChannel * this->sparseFractions_ / sparseNumerator_;
            int activeSize = maxActive +1;
            uint8_t threshold_ = min;
            uint8_t thresholdStep_ = 1;
            while (activeSize > maxActive) {
                activeSize = 0;
                for(int i =0;i < oneChannel;i++)
                {
                    int index = ch * oneChannel + i;
                    int delta = raw->data_[index] - threshold_;
                    if(0 < delta ){
                        blob->data_[index] = true;
                        activeSize++;
                    }else{
                        blob->data_[index] = false;
                    }
                }
                threshold_ += thresholdStep_;
            }
        }
        blobs_.push_back(blob);
    }
#if 0
    INFO_VAR(pinchs_.size());
    for(int index = 0; index < pinchs_.size();index++) {
        auto pinch = pinchs_[index];
        TRACE_VAR(pinch->size_);
        auto blob = shared_ptr<Blob<bool>>(new Blob<bool>(pinch->w_/this->w_,pinch->h_/this->h_,pinch->ch_));
        int blobIndex = 0;
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
            if(blobIndex++ >= blob->size_) {
                continue;
            }
            blob->data_[blobIndex] = memory_->filter3x3(memIndex);
        }
        blobs_.push_back(blob);
    }
#endif
    INFO_VAR(blobs_.size());
    INFO_VAR("finnish LeibnizLayer::forward");
}


/**
 * dump to png
 * @return None.
 **/
void LeibnizLayer::dump(void){
    INFO_VAR(blobs_.size());
    for (auto blob:blobs_) {
        blob->dump(typeid(this).name());
    }
}

/**
 * get ptr
 * @return None.
 **/
shared_ptr<Blob<bool>> LeibnizLayer::getBlob(const LayerBase* who)
{
    int i = 0;
    TRACE_VAR(top_.size());
    for(auto top:top_)
    {
        if(who == top.get())
        {
            TRACE_VAR(blobs_.size());
            if(blobs_.size() > i)
            {
                return blobs_[i];
            }
            else
            {
                FATAL_VAR("fatal errro");
            }
        }
        i++;
    }
    return nullptr;
}

