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
#pragma once
#include <fstream>
#include <memory>
#include <vector>
#include <string>
using namespace std;
namespace Wator {
    /**
     * Blob.
     **/
    template <typename T>
    class Blob {
    public:
        /**
         * Constructor
        **/
        explicit Blob();

        /**
         * Constructor
         * @param [in] size
         **/
        explicit Blob(int size)
        :size_(size)
        ,data_(new T[size])
        {
            memset(data_,0x0,size_);
        }

        /**
         * Constructor
         * @param [in] size 
         **/
        explicit Blob(int width,int height,int channel)
        :w_(width)
        ,h_(height)
        ,ch_(channel)
        ,size_(w_*h_*ch_)
        ,data_(new T[size_])
        {
            memset(data_,0x0,size_);
        }

        /**
         * Constructor
         * @param [in] size
         **/
        explicit Blob(const vector<shared_ptr<Blob<T>>> &conv);
        
        
        
        ~Blob()
        {
            delete [] data_;
        }
 
        /**
         * splite blob to grid.
         * @param [in] gw grid width
         * @param [in] gh grid height
         * @return None.
         **/
        shared_ptr<Blob<T>> grid(int startX,int startY,int gw,int gh);
       
        /**
         * cut Chi,which is not connect by other.
         * @return None.
         **/
        void cutChi(void);

        /**
         * @param [out] area
         * @param [in] x
         * @param [in] y
         * @param [in] ch
         * @return None.
         **/
        void neighbor(shared_ptr<Blob<T>> area,int x,int y,int ch);
        
        /**
         * @param [in] x
         * @param [in] y
         * @param [in] ch
         * @return None.
         **/
        uint16_t label(uint16_t x,uint16_t y,uint16_t ch);
        
        /**
         * splite to connect area.
         * @return None.
         **/
        vector<shared_ptr<Blob<T>>> splite(void);
        
        /**
         * dump to png
         * @param [in] name
         * @return None.
         **/
        void dump(const string &name);
        
    public:
        uint16_t w_;
        uint16_t h_;
        uint16_t ch_;
        uint32_t size_ ;
        T *data_;
        uint16_t minX_;
        uint16_t minY_;
        uint16_t maxX_;
        uint16_t maxY_;
        map<uint32_t,uint16_t> labels_;
        uint16_t labelCounter_;
    };
}
