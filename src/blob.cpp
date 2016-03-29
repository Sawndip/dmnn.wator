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

template void Blob<bool>::dump(const string &name);

