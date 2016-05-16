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
using namespace std;
/**
 * dump to png
 * @return None.
 **/
template <typename T> void Blob<T>::dump(const string &name){
    INFO_VAR(this->w_);
    INFO_VAR(this->h_);
    INFO_VAR(this->ch_);
    
    vector<shared_ptr<cv::Mat>> mats;
    for (int ch = 0; ch < this->ch_; ch++) {
        shared_ptr<cv::Mat> mat(new cv::Mat(this->h_,this->w_,CV_8UC3,cv::Scalar::all(0)));
        mats.push_back(mat);
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
                }
            }
        }
    }
    static int counter = 0;
    for (int ch = 0; ch < this->ch_; ch++) {
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
        path += ".png";
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
template <typename T> shared_ptr<Blob<T>> Blob<T>::grid(int gw,int gh)
{
    int roundW = (this->w_/gw) * gw;
    int roundH = (this->h_/gh) * gh;
    INFO_VAR(roundW);
    INFO_VAR(roundH);
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
                index2 += y*this->w_ + x;
                TRACE_VAR(index2);
                if (index >= gridBlob->size_|| index2 >= this->size_) {
                    // 无法整除的最后几行，不能的到下一层的完整输出，省略。
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
                    // left line
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
                    // center line
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
                    // right line
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
template <typename T> vector<shared_ptr<Blob<T>>> Blob<T>::splite(void)
{
    vector<shared_ptr<Blob<T>>> areas;
    auto area = shared_ptr<Blob<T>> (new Blob<T>(this->w_,this->h_,this->ch_));
    areas.push_back(area);
    return areas;
}


template void Blob<bool>::dump(const string &name);
template shared_ptr<Blob<uint8_t>> Blob<uint8_t>::grid(int gridW,int gh);
template shared_ptr<Blob<bool>> Blob<bool>::grid(int gridW,int gh);
template void Blob<bool>::cutChi(void);
template vector<shared_ptr<Blob<bool>>> Blob<bool>::splite(void);


