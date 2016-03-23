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
    class V1CortexLayer;
    class CoulombLayer :public LayerBase {
        friend class ImageLayer;
    public:
        /**
         * Constructor
         **/
        explicit CoulombLayer();

        /**
         * Connect a Layer to Net.
         * @param [in] layer
         **/
        V1CortexLayer& operator << (V1CortexLayer &layer);

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
         * aspect
         * @return None.
         **/
        virtual void aspect(int w,int h,int ch);
    
        /**
         * get ptr
         * @return None.
         **/
        shared_ptr<Blob<bool>> getBlob(const LayerBase* who);

    protected:
        /**
         * update
         * @return None.
         **/
        virtual void update(void);
    private:
        /**
         * update
         * @return None.
         **/
        void updateW(void);

        /**
         * dump to png
         * @return None.
         **/
        void dump(void);
    private:
        const int w_ = 3;
        const int h_ = 3;
        int size_ = 0;
        float max_ = 0;
        float min_ = INT32_MAX;
        
        vector<float> activeWeight_;
        vector<float> deactiveWeight_;
        vector<float> weight_;
        
        vector<shared_ptr<Blob<float>>> blobsRaw_;
        vector<shared_ptr<Blob<bool>>> blobs_;

        // K Coulomb
        vector<float> kCoulomb_;
        
        int  threshold_ = 1;
        const int  thresholdStep_ = 1;
        
        int wGrid_;
        int hGrid_;
        int chGrid_;
    };

}
