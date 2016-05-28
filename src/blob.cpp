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


#include <opencv/highgui.h>
#include <vector>
#include <memory>
#include <climits>
using namespace std;



/**
 * Constructor
 * @param [in] size
 **/
template <typename T> Blob<T>::Blob(const vector<shared_ptr<Blob<T>>> &conv)
{
    this->w_ = UINT16_MAX;
    this->h_ = UINT16_MAX;
    this->ch_ = 0;
    for(const auto &blob:conv) {
        if(this->w_ > blob->w_) {
            this->w_ = blob->w_;
        }
        if(this->h_ > blob->h_) {
            this->h_ = blob->h_;
        }
        if(this->ch_ < blob->ch_) {
            this->ch_ = blob->ch_;
        }
    }
    INFO_VAR(this->w_);
    INFO_VAR(this->h_);
    INFO_VAR(this->ch_);
    if(this->w_ != UINT16_MAX && this->h_ != UINT16_MAX && this->ch_ != 0) {
        size_ = this->w_* this->h_ * this->ch_;
        data_ = new T[size_];
        //memset(data_,0x0,sizeof(T)*size_);
        for (int ch = 0; ch < this->ch_; ch++) {
            for (int x = 0; x < this->w_; x++) {
                for (int y = 0; y < this->h_; y++) {
                    int index = ch * this->w_ * this->h_ + y * this->w_ + x;
                    bool convBit = false;
                    for(const auto &blob:conv) {
                        int index2 = ch * blob->w_ * blob->h_ + y* blob->w_ +x;
                        if(blob->data_[index2]) {
                            convBit = true;;
                        }
                    }
                    this->data_[index] = convBit;
                }
            }
        }
   }
}


/**
 * dump to png
 * @return None.
 **/
template <typename T> void Blob<T>::dump(const string &name){
    INFO_VAR(this->w_);
    INFO_VAR(this->h_);
    INFO_VAR(this->ch_);
    
    vector<shared_ptr<cv::Mat>> mats;
    vector<bool> writeOut;
    for (int ch = 0; ch < this->ch_; ch++) {
        shared_ptr<cv::Mat> mat(new cv::Mat(this->h_,this->w_,CV_8UC3,cv::Scalar::all(0)));
        mats.push_back(mat);
        writeOut.push_back(false);
    }
    for (int x = 0; x < this->w_; x++) {
        for (int y = 0; y < this->h_; y++) {
            if(3 ==this->ch_) {
                for (int ch = 0; ch < this->ch_; ch++) {
                    int index = ch * this->w_ * this->h_ + y * this->w_ + x;
                    auto value = this->data_[index];
                    TRACE_VAR(index);
                    TRACE_VAR(value);
                    if (value) {
                        mats[ch]->at<cv::Vec3b>(y,x)[ch] = 0xff;
                        writeOut[ch] = true;
                    }
                }
            }
            if (1 ==this->ch_) {
                int index = y*this->w_ + x;
                auto value = this->data_[index];
                INFO_VAR(index);
                INFO_VAR(value);
                if (value) {
                    mats[0]->at<cv::Vec3b>(y,x)[0] = 0xff;
                    mats[0]->at<cv::Vec3b>(y,x)[1] = 0xff;
                    mats[0]->at<cv::Vec3b>(y,x)[2] = 0xff;
                    writeOut[0] = true;
                }
            }
        }
    }
    static int counter = 0;
    for (int ch = 0; ch < this->ch_; ch++) {
        if(writeOut[ch] == false) {
            continue;
        }
        string path = "dump.image.";
        path += name;
        path += ".";
        path += std::to_string(counter);
        if(0 == ch){
            path += ".B.";
        }
        if(1 == ch){
            path += ".G.";
        }
        if(2 == ch){
            path += ".R.";
        }
        path += "png";
        cv::imwrite(path ,*mats[ch]);
    }
    ++counter;
}

/**
 * splite blob to grid.
 * @param [in] gw grid width
 * @param [in] gh grid height
 * @return None.
 **/
template <typename T> shared_ptr<Blob<T>> Blob<T>::grid(int startX,int startY,int gw,int gh)
{
    int roundW = ((this->w_ - startX)/gw) * gw;
    int roundH = ((this->h_ - startY)/gh) * gh;
    INFO_VAR(roundW);
    INFO_VAR(roundH);
    if(roundW < 0 || roundH < 0) {
        roundW = 0;
        roundH = 0;
    }
    auto gridBlob = shared_ptr<Blob<T>> (new Blob<T>(roundW,roundH,this->ch_));
    for (int ch = 0; ch < this->ch_; ch++) {
        for (int y = 0; y < roundH; y++) {
            for (int x = 0; x < roundW; x++) {
                int grid = (y/gh) * (roundW/gw) + (x/gw) ;
                int index = ch * roundW * roundH;
                index += grid * gw * gh;
                index += (y%gh)*gw  + x%gw ;
                TRACE_VAR(index);
                TRACE_VAR(x);
                TRACE_VAR(y);
                int index2 = ch * this->w_ * this->h_;
                index2 += (startY+y) * this->w_ + x + startX;
                TRACE_VAR(index2);
                if (index >= gridBlob->size_|| index2 >= this->size_) {
                    INFO_VAR(gw);
                    INFO_VAR(gh);
                    INFO_VAR(grid);
                    INFO_VAR(x);
                    INFO_VAR(y);
                    INFO_VAR(index);
                    INFO_VAR(index2);
                    INFO_VAR(this->w_);
                    INFO_VAR(this->h_);
                    INFO_VAR(roundW);
                    INFO_VAR(roundH);
                    INFO_VAR(this->size_);
                    INFO_VAR(gridBlob->size_);
                    continue;
                }
                gridBlob->data_[index] = this->data_[index2];
            }
        }
    }
    return gridBlob;
}

/**
 * cutChi.
 * @return None.
 **/
template <typename T> void Blob<T>::cutChi(void)
{
    for (int ch = 0; ch < this->ch_; ch++) {
        for (int y = 0; y < this->h_; y++) {
            for (int x = 0; x < this->w_; x++) {
                int index = ch * this->w_ * this->h_;
                index += y*this->w_ + x;
                TRACE_VAR(index);
                bool Chi = true;
                if(this->data_[index]) {
                    // left cols
                    if(x-1 >= 0) {
                        int index1 = ch * this->w_ * this->h_;
                        index1 += y*this->w_ + x -1;
                        if(this->data_[index1]) {
                            Chi = false;
                        }
                        if(y-1 >= 0) {
                            int index2 = ch * this->w_ * this->h_;
                            index2 += (y-1)*this->w_ + x -1;
                            if(this->data_[index2]) {
                                Chi = false;
                            }
                        }
                        if(y+1 <= this->h_) {
                            int index2 = ch * this->w_ * this->h_;
                            index2 += (y+1)*this->w_ + x-1;
                            if(this->data_[index2]) {
                                Chi = false;
                            }
                        }
                    }
                    // center cols
                    if(y-1 >= 0) {
                        int index1 = ch * this->w_ * this->h_;
                        index1 += (y-1)*this->w_ + x;
                        if(this->data_[index1]) {
                            Chi = false;
                        }
                    }
                    if(y+1 <= this->h_) {
                        int index1 = ch * this->w_ * this->h_;
                        index1 += (y+1)*this->w_ + x;
                        if(this->data_[index1]) {
                            Chi = false;
                        }
                    }
                    // right cols
                    if(x+1 <= this->w_) {
                        int index1 = ch * this->w_ * this->h_;
                        index1 += y*this->w_ + x +1;
                        if(this->data_[index1]) {
                            Chi = false;
                        }
                        if(y-1 >= 0) {
                            int index2 = ch * this->w_ * this->h_;
                            index2 += (y-1)*this->w_ + x +1;
                            if(this->data_[index2]) {
                                Chi = false;
                            }
                        }
                        if(y+1 <= this->h_) {
                            int index2 = ch * this->w_ * this->h_;
                            index2 += (y+1)*this->w_ + x+1;
                            if(this->data_[index2]) {
                                Chi = false;
                            }
                        }
                    }
                }
                if(Chi) {
                    this->data_[index] = false;
                }
            }
        }
    }
}


/**
 * splite to connect area.
 * @return None.
 **/
/*
template <typename T> vector<shared_ptr<Blob<T>>> Blob<T>::splite(void)
{
    vector<shared_ptr<Blob<T>>> areas;
    for (int ch = 0; ch < this->ch_; ch++) {
        for (int y = 0; y < this->h_; y++) {
            for (int x = 0; x < this->w_; x++) {
                int index = ch * this->w_ * this->h_;
                index += y*this->w_ + x;
                if(this->data_[index]) {
                    auto area = shared_ptr<Blob<T>> (new Blob<T>(this->w_,this->h_,this->ch_));
                    this->data_[index] = false;
                    area->data_[index] = true;
                    minX_ = maxX_ = x;
                    minY_ = maxY_ = y;
                    this->neighbor(area,x,y,ch);
                    int areaX = (maxX_-minX_)*(maxY_-minY_);
                    if(areaX > (this->w_*this->h_)/(10*10)) {
                        areas.push_back(area);
                    }
                }
            }
        }
    }
    return areas;
}
*/

template <typename T> vector<shared_ptr<Blob<T>>> Blob<T>::splite(void)
{
    labels_.clear();
    areasRaw_.clear();
    areaMasks_.clear();
    areasMaxMin_.clear();
    for (uint16_t ch = 0; ch < this->ch_; ch++) {
        labelCounter_ = 0;
        for (uint16_t y = 0; y < this->h_; y++) {
            for (uint16_t x = 0; x < this->w_; x++) {
                uint16_t index = ch * this->w_ * this->h_;
                index += y*this->w_ + x;
                if(this->data_[index]) {
                    uint32_t labelKey = ((x<<16)&0xffff0000)|(y&0xffff);
                    uint32_t labelValue = this->label(x,y,ch);
                    labels_[labelKey] = labelValue;
                    //INFO_VAR(labelValue);
                    auto ir = areaMasks_.find(labelValue);
                    if(ir == areaMasks_.end()) {
                        vector<uint32_t> area ={labelKey};
                        areaMasks_[labelValue] = area;
                    } else {
                        ir->second.push_back(labelKey);
                    }
                    auto ir2 = areasMaxMin_.find(labelValue);
                    if(ir2 == areasMaxMin_.end()) {
                        areasMaxMin_[labelValue] = {x,y,x,y};
                    } else {
                        if(x < ir2->second[0]){
                            ir2->second[0] = x;
                        }
                        if(y < ir2->second[1]){
                            ir2->second[1] = y;
                        }
                        if(x > ir2->second[2]){
                            ir2->second[2] = x;
                        }
                        if(y > ir2->second[3]){
                            ir2->second[3] = y;
                        }
                    }
                }
            }
        }
    }
    this->cutSmall();
    INFO_VAR(areaMasks_.size());
    vector<shared_ptr<Blob<T>>> areas;
    for(auto area:areasMaxMin_) {
        uint32_t areaXY = (area.second[2] - area.second[0])*(area.second[3] - area.second[1]);
        uint32_t areaTheshold = (this->w_*this->h_)/(64*64);
        if(areaXY > areaTheshold) {
            TRACE_VAR(area.second[0]);
            TRACE_VAR(area.second[1]);
            TRACE_VAR(area.second[2]);
            TRACE_VAR(area.second[3]);
            TRACE_VAR(areaXY);
            TRACE_VAR(areaTheshold);
            auto label = area.first;
            auto irLabel = areaMasks_.find(label);
        }
    }
    INFO_VAR(areasMaxMin_.size());
    INFO_VAR(areas.size());
    return areas;
}

/**
 * @param [in] x
 * @param [in] y
 * @param [in] ch
 * @return None.
 **/
template <typename T> uint32_t Blob<T>::label(uint16_t x,uint16_t y,uint16_t ch)
{
    // left top
    if(x-1 >= 0 && y-1 >= 0) {
        int index = ch * this->w_ * this->h_;
        index += (y-1)*this->w_ + x -1;
        if(this->data_[index]) {
            uint32_t labelKey = (((x-1)<<16)&0xffff0000)|((y-1)&0xffff);
            auto ir = labels_.find(labelKey);
            if(ir!= labels_.end()) {
                return ir->second;
            }
        }
    }
    // left center
    if(x-1 >= 0 /* && y*/) {
        int index = ch * this->w_ * this->h_;
        index += (y)*this->w_ + x -1;
        if(this->data_[index]) {
            uint32_t labelKey = (((x-1)<<16)&0xffff0000)|((y)&0xffff);
            auto ir = labels_.find(labelKey);
            if(ir!= labels_.end()) {
                return ir->second;
            }
        }
    }
    // top center.
    if(/*x && */ y-1 >= 0) {
        int index = ch * this->w_ * this->h_;
        index += (y-1)*this->w_ + x;
        if(this->data_[index]) {
            uint32_t labelKey = (((x)<<16)&0xffff0000)|((y-1)&0xffff);
            auto ir = labels_.find(labelKey);
            if(ir!= labels_.end()) {
                return ir->second;
            }
        }
    }
    return labelCounter_++;
}

/**
 * cut Small connected area.
 * @return None.
 **/
template <typename T> void Blob<T>::cutSmall() {
    for(auto area:areasMaxMin_) {
        uint32_t areaXY = (area.second[2] - area.second[0])*(area.second[3] - area.second[1]);
        uint32_t areaTheshold = (this->w_*this->h_)/(64*64);
        if(areaXY < areaTheshold) {
            TRACE_VAR(area.second[0]);
            TRACE_VAR(area.second[1]);
            TRACE_VAR(area.second[2]);
            TRACE_VAR(area.second[3]);
            TRACE_VAR(areaXY);
            TRACE_VAR(areaTheshold);
            auto label = area.first;
            auto irLabel = areaMasks_.find(label);
            if(irLabel != areaMasks_.end()) {
                
            }
        }
    }
    INFO_VAR(areasMaxMin_.size());
}



/**
 * @param [out] area
 * @param [in] x
 * @param [in] y
 * @param [in] ch
 * @return None.
 **/
template <typename T> void Blob<T>::neighbor(shared_ptr<Blob<T>> area,int x,int y,int ch)
{
    // left cols
    if(x-1 >= 0) {
        // top
        if(y-1 >= 0) {
            int index2 = ch * this->w_ * this->h_;
            index2 += (y-1)*this->w_ + x -1;
            if(this->data_[index2]) {
                this->data_[index2] = false;
                area->data_[index2] = true;
                this->neighbor(area,x-1,y-1,ch);
                this->minX_--;
                this->minY_--;
            }
        }
        // center
        int index = ch * this->w_ * this->h_;
        index += y*this->w_ + x -1;
        if(this->data_[index]) {
            this->data_[index] = false;
            area->data_[index] = true;
            this->neighbor(area,x-1,y,ch);
            this->minX_--;
        }
        // down
        if(y+1 <= this->h_) {
            int index2 = ch * this->w_ * this->h_;
            index2 += (y+1)*this->w_ + x-1;
            if(this->data_[index2]) {
                this->data_[index2] = false;
                area->data_[index2] = true;
                this->neighbor(area,x-1,y+1,ch);
                this->minX_--;
                this->maxY_++;
            }
        }
    }
    
    // center cols
    // top
    if(y-1 >= 0) {
        int index1 = ch * this->w_ * this->h_;
        index1 += (y-1)*this->w_ + x;
        if(this->data_[index1]) {
            this->data_[index1] = false;
            area->data_[index1] = true;
            this->neighbor(area,x,y-1,ch);
            this->minY_--;
        }
    }
    // down
    if(y+1 <= this->h_) {
        int index1 = ch * this->w_ * this->h_;
        index1 += (y+1)*this->w_ + x;
        if(this->data_[index1]) {
            this->data_[index1] = false;
            area->data_[index1] = true;
            this->neighbor(area,x,y+1,ch);
            this->maxY_++;
        }
    }
    
    // right cols
    if(x+1 <= this->w_) {
        // top
        if(y-1 >= 0) {
            int index2 = ch * this->w_ * this->h_;
            index2 += (y-1)*this->w_ + x +1;
            if(this->data_[index2]) {
                this->data_[index2] = false;
                area->data_[index2] = true;
                this->neighbor(area,x+1,y-1,ch);
                this->maxX_++;
                this->minY_--;
            }
        }
        // center
        int index1 = ch * this->w_ * this->h_;
        index1 += y*this->w_ + x +1;
        if(this->data_[index1]) {
            this->data_[index1] = false;
            area->data_[index1] = true;
            this->neighbor(area,x+1,y,ch);
            this->maxX_++;
        }
        // down
        if(y+1 <= this->h_) {
            int index2 = ch * this->w_ * this->h_;
            index2 += (y+1)*this->w_ + x+1;
            if(this->data_[index2]) {
                this->data_[index2] = false;
                area->data_[index2] = true;
                this->neighbor(area,x+1,y+1,ch);
                this->maxX_++;
                this->maxY_++;
            }
        }
    }
}


template Blob<bool>::Blob(const vector<shared_ptr<Blob<bool>>> &conv);

template void Blob<bool>::dump(const string &name);
template shared_ptr<Blob<uint8_t>> Blob<uint8_t>::grid(int startX,int startY,int gw,int gh);
template shared_ptr<Blob<bool>> Blob<bool>::grid(int startX,int startY,int gw,int gh);
template void Blob<bool>::cutChi(void);
template vector<shared_ptr<Blob<bool>>> Blob<bool>::splite(void);
template void Blob<bool>::neighbor(shared_ptr<Blob<bool>> conn,int x,int y,int ch);

