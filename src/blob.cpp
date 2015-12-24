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
/**
 * dump to png
 * @return None.
 **/
template <typename T> void Blob<T>::dump(void){
    INFO_VAR(this->w_);
    INFO_VAR(this->h_);
    INFO_VAR(this->ch_);
    cv::Mat image(this->h_,this->w_,CV_8UC3);
    for (int x = 0; x < this->w_; x++) {
        for (int y = 0; y < this->h_; y++) {
            if(3 ==this->ch_) {
                for (int ch = 0; ch < this->ch_; ch++) {
                    int index = ch*this->w_* this->h_ + y*this->w_ + x;
                    auto value = this->data_[index];
                    TRACE_VAR(index);
                    TRACE_VAR(value);
                    if (value) {
                        image.at<cv::Vec3b>(y,x)[ch] = 0xff;
                    } else {
                        image.at<cv::Vec3b>(y,x)[ch] = 0x0;
                    }
                }
            }
            if (1 ==this->ch_) {
                int index = y*this->w_ + x;
                auto value = this->data_[index];
                TRACE_VAR(index);
                TRACE_VAR(value);
                if (value) {
                    image.at<cv::Vec3b>(y,x)[0] = 0xff;
                    image.at<cv::Vec3b>(y,x)[1] = 0xff;
                    image.at<cv::Vec3b>(y,x)[2] = 0xff;
                } else {
                    image.at<cv::Vec3b>(y,x)[0] = 0x0;
                    image.at<cv::Vec3b>(y,x)[1] = 0x0;
                    image.at<cv::Vec3b>(y,x)[2] = 0x0;
                }
            }
        }
    }
    cv::imwrite("dump.image.jpg",image);
}

template void Blob<bool>::dump();

