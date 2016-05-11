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
using namespace std;
#include "wator/layer.hpp"

namespace Wator {
    class GanglionLayer;
    class EinsteinLayer :public LayerBase {
        friend class ImageLayer;
    public:
        /**
         * Constructor
         **/
        explicit EinsteinLayer();

        /**
         * round
         * @return None.
         **/
        virtual void round(void);
        
        /**
         * forward
         * @return None.
         **/
        virtual void forward(void);

        /**
         * get ptr
         * @return None.
         **/
        shared_ptr<Blob<bool>> getBlob(const LayerBase* who);
        
        
        /**
         * get ptr
         * @return None.
         **/
        shared_ptr<Blob<bool>> getBlob2X2(const LayerBase* who);

        /**
         * get ptr
         * @return None.
         **/
        shared_ptr<Blob<bool>> getBlob4X4(const LayerBase* who);
        
    protected:
        /**
         * forward1
         * @return None.
         **/
        void forward1(void);
     
        /**
         * forward2
         * @return None.
         **/
        void forward2(void);

        /**
         * update
         * @return None.
         **/
        virtual void update(void);
    private:
        /**
         * dump to png
         * @return None.
         **/
        void dump(void);
        
        /**
         * cal 4 vecotor.
         * @return None.
         **/
        void cal4Vec(uint8_t *start,uint8_t &maxDiff,uint8_t &avg);

        /**
         * cut point that is not connect to anthers.
         * @return None.
         **/
        void cutIsolation(void);
 
        /**
         * cut point that is not connect to anthers.
         * @return None.
         **/
        void cutIsolation(shared_ptr<Blob<bool>> blob);
       
    private:
        const int w_ = 2;
        const int h_ = 2;
        
        
        vector<shared_ptr<Blob<uint8_t>>> blobsRaw2X2_;
        vector<shared_ptr<Blob<uint8_t>>> blobsRaw4X4_;
        
        vector<shared_ptr<Blob<bool>>> blobs2x2_;
        vector<shared_ptr<Blob<bool>>> blobs4x4_;
        vector<shared_ptr<Blob<bool>>> blobs_;
        
        
        
        int wGrid2x2_;
        int hGrid2x2_;
  
        int wGrid4x4_;
        int hGrid4x4_;
        
        // 20%
        const int sparseFractions_ = 20;
        const int sparseNumerator_ = 100;
        
    };

}
