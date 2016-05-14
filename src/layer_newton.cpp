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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;

static float const fConstCoulombDiff = 0.0001;


/**
 * Constructor
 **/
NewtonLayer::NewtonLayer()
{
}

/**
 * update
 * @return None.
 **/
void NewtonLayer::update(void)
{
    INFO_VAR("finnish NewtonLayer::update");
}


/**
 * round
 * @return None.
 **/
void NewtonLayer::round(void)
{
    this->forward();
    this->update();
    this->cutIsolation();
    this->dump();
    INFO_VAR("finnish NewtonLayer::round");
    LayerBase::round();
}


/*
 mat
 x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12
 y1,y2,y3,y4,y5,y6,y7,y8,y9,y10,y11,y12
 z1,z2,z3,z4,z5,z6,z7,z8,z9,z10,z11,z12
 u1,u2,u3,u4,u5,u6,u7,u8,u9,u10,u11,u12
 v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12
 w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,w11,w12
 r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12
 s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12
 t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12
 o1,o2,o3,o4,o5,o6,o7,o8,o9,o10,o11,o12
 p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12
 q1,q2,q3,q4,q5,q6,q7,q8,q9,q10,q11,q12

 
 ------------------
 image blob
 x1,x2,x3,y1,y2,y3,z1,z2,z3,
 x4,x5,x6,y4,y5,y6,z4,z5,z6,
 x7,x8,x9,y7,y8,y9,z7,z8,z9,
 x10,x11,x12,y10,y11,y12,z10,z11,z12,
 u1,u2,u3,v1,v2,v3,w1,w2,w3,
 u4,u5,u6,v4,v5,v6,w4,w5,w6,
 u7,u8,u9,v7,v8,v9,w7,w8,w9,
 u10,u11,u12,v10,v11,v12,w10,w11,w12,
 r1,r2,r3,s1,s2,s3,t1,t2,t3,
 r4,r5,r6,s4,s5,s6,t4,t5,t6,
 r10,r11,r12,s10,s11,s12,t10,t11,t12,
 
 o1,o2,o3,p1,p2,p3,q1,q2,q3
 o4,o5,o6,p4,p5,p6,q4,q5,q6
 o7,o8,o9,p7,p8,p9,q7,q8,q9
 o10,o11,o12,p10,p11,p12,q10,q11,q12

 ------------------
 coulomb blob
 xyz123,xyz456,xyz789,xyz101112,
 uvw123,uvw456,uvw789,uvw101112,
 rst123,rst456,rst789,rst101112,
 opq123,opq456,opq789,opq101112
 */


/**
 * cal 4 vecotor.
 * @return None.
 **/
void NewtonLayer::cal4Vec(uint8_t *start,uint8_t &maxDiff,uint8_t &avg)
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
    
    avg = (start[0] + start[1] + start[2] + start[3])/4;
}

/**
 * forward
 * @return None.
 **/
void NewtonLayer::forward(void)
{
    blobsRaw2X2_.clear();
    blobsRaw4X4_.clear();
    blobs2x2_.clear();
    blobs4x4_.clear();
    blobs_.clear();

    forward2();

    INFO_VAR(blobs_.size());
    INFO_VAR("finnish NewtonLayer::forward");
}


/**
 * forward1
 * @return None.
 **/
void NewtonLayer::forward1(void)
{
#if 0
    for(auto btm:bottom_){
        // 2x2
        auto input = dynamic_pointer_cast<LayerInput>(btm);
        auto inBlob_orig = input->getBlob(this);
        auto inBlob = inBlob_orig->grid(this->w_,this->h_);
        INFO_VAR(inBlob->w_);
        INFO_VAR(inBlob->h_);
        INFO_VAR(inBlob->ch_);
        this->wGrid2x2_ = inBlob->w_/this->w_;
        this->hGrid2x2_ = inBlob->h_/this->h_;
        INFO_VAR(this->wGrid2x2_);
        INFO_VAR(this->hGrid2x2_);
        auto raw2x2 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
        auto nextInput2x2 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
        for(int i = 0 ;i < raw2x2->size_ ;i++) {
            uint8_t maxDiff = 0;
            uint8_t avg = 0;
            int index = i*this->w_*this->h_;
            cal4Vec(&(inBlob->data_[index]),maxDiff,avg);
            raw2x2->data_[i] = maxDiff;
            nextInput2x2->data_[i] = avg;
        }
        blobsRaw2X2_.push_back(raw2x2);
        // 4x4
        this->wGrid4x4_ =this->wGrid2x2_/2;
        this->hGrid4x4_ =this->hGrid2x2_/2;
        INFO_VAR(this->wGrid4x4_);
        INFO_VAR(this->hGrid4x4_);
        auto swap4x4 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
        for(int channel = 0 ; channel < inBlob->ch_;channel++){
            for(int y = 0;y < this->hGrid2x2_;y++){
                for(int x = 0;x < this->wGrid2x2_;x++){
                    int indexInput = channel *(this->hGrid2x2_ * this->wGrid2x2_) + y*this->wGrid2x2_ + x;
                    auto byte = nextInput2x2->data_[indexInput];
                    
                    const int grid = ((y/2) * (wGrid2x2_/2))+ (x/2) ;
                    TRACE_VAR(grid);
                    const int groundW = (wGrid2x2_/2)*2;
                    const int groundH = (hGrid2x2_/2)*2;
                    if(groundW < x || groundH < y) {
                        // out side of last grid.
                        continue;
                    }
                    int index = channel * groundW * groundH;
                    index += grid * 2 * 2;
                    index += (y%2)*2  + x%2 ;
                    TRACE_VAR(index);
                    if(index > swap4x4->size_) {
                        INFO_VAR(index);
                        INFO_VAR(swap4x4->size_);
                        continue;
                    }
                    swap4x4->data_[index] = byte;
               }
            }
        }
        //
        auto raw4x4 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid4x4_,this->hGrid4x4_,inBlob->ch_));
        auto nextInput4x4 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid4x4_,this->hGrid4x4_,inBlob->ch_));
        for(int i = 0 ;i < raw4x4->size_ ;i++) {
            uint8_t maxDiff = 0;
            uint8_t avg = 0;
            int index = i*this->w_*this->h_;
            cal4Vec(&(swap4x4->data_[index]),maxDiff,avg);
            raw4x4->data_[i] = maxDiff;
            nextInput4x4->data_[i] = avg;
        }
        blobsRaw4X4_.push_back(raw4x4);

        for (auto top:top_) {
            auto leib = dynamic_pointer_cast<LeibnizLayer>(top);

            auto blob2x2 = shared_ptr<Blob<bool>>(new Blob<bool>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
            auto gridW2x2 = this->wGrid2x2_/(2*leib->iW_);
            auto gridH2x2 = this->hGrid2x2_/(2*leib->iH_);
            for (int ch = 0; ch < inBlob->ch_; ch++) {
                for (int x = 0;x < gridW2x2;x++){
                    for (int y = 0;y < gridH2x2;y++){
                        TRACE_VAR(activeSize);
                        const int maxActive = leib->iSparse_;
                        int activeSize = maxActive +1;
                        int iCounter = 0;
                        uint8_t max_ = 0;
                        uint8_t min_ = 255;
                        uint8_t threshold_ = 0;
                        uint8_t thresholdStep_ = 1;
                        while (activeSize > maxActive) {
                            activeSize = 0;
                            for(int x2 = 0 ;x2 < 2*leib->iW_ ;x2++) {
                                for(int y2 = 0 ;y2 < 2*leib->iH_ ;y2++) {
                                    /* index */
                                    auto index = ch * this->wGrid2x2_* this->hGrid2x2_ + (y * 2*leib->iH_+y2) * this->wGrid2x2_ + x*2*leib->iW_ + x2;
                                    if(iCounter == 0) {
                                        //if(0) {
                                        if(raw2x2->data_[index] > max_) {
                                            max_ = raw2x2->data_[index];
                                        }
                                        if(raw2x2->data_[index] < min_) {
                                            min_ = raw2x2->data_[index];
                                        }
                                    } else {
                                        int delta = raw2x2->data_[index] - threshold_;
                                        TRACE_VAR(delta);
                                        TRACE_VAR((int)raw2x2->data_[index])
                                        if(0 < delta ){
                                            blob2x2->data_[index] = true;
                                            activeSize++;
                                        }else{
                                            blob2x2->data_[index] = false;
                                        }
                                    }
                                }
                            }
                            if(iCounter == 0) {
                                // first time count
                                threshold_ = min_;
                                activeSize = maxActive +1;
                                TRACE_VAR((double)max_);
                                TRACE_VAR((double)min_);
                            } else {
                                threshold_ += thresholdStep_;
                            }
                            TRACE_VAR((double)max_);
                            TRACE_VAR((double)min_);
                            TRACE_VAR(iCounter);
                            TRACE_VAR(activeSize);
                            TRACE_VAR((double)threshold_);
                            iCounter++;
                        }
                        TRACE_VAR(activeSize);
                        TRACE_VAR(maxActive);
                    }
                }
            }
            blobs2x2_.push_back(blob2x2);
            
            auto blob4x4 = shared_ptr<Blob<bool>>(new Blob<bool>(this->wGrid4x4_,this->hGrid4x4_,inBlob->ch_));
            auto gridW4x4 = this->wGrid4x4_/leib->iW_;
            auto gridH4x4 = this->hGrid4x4_/leib->iH_;
            for (int ch = 0; ch < inBlob->ch_; ch++) {
                for (int x = 0;x < gridW4x4;x++){
                    for (int y = 0;y < gridH4x4;y++){
                        TRACE_VAR(activeSize);
                        const int maxActive = leib->iSparse_;
                        int activeSize = maxActive +1;
                        int iCounter = 0;
                        uint8_t max_ = 0;
                        uint8_t min_ = 255;
                        uint8_t threshold_ = 0;
                        uint8_t thresholdStep_ = 1;
                        while (activeSize > maxActive) {
                            activeSize = 0;
                            for(int x2 = 0 ;x2 < leib->iW_ ;x2++) {
                                for(int y2 = 0 ;y2 < leib->iH_ ;y2++) {
                                    /* index */
                                    auto index = ch * this->wGrid4x4_* this->hGrid4x4_ + (y * leib->iH_+y2) * this->wGrid4x4_ + x*leib->iW_ + x2;
                                    if(iCounter == 0) {
                                        //if(0) {
                                        if(raw4x4->data_[index] > max_) {
                                            max_ = raw4x4->data_[index];
                                        }
                                        if(raw4x4->data_[index] < min_) {
                                            min_ = raw4x4->data_[index];
                                        }
                                    } else {
                                        int delta = raw4x4->data_[index] - threshold_;
                                        TRACE_VAR(delta);
                                        TRACE_VAR((int)raw4x4->data_[index])
                                        if(0 < delta ){
                                            blob4x4->data_[index] = true;
                                            activeSize++;
                                        }else{
                                            blob4x4->data_[index] = false;
                                        }
                                    }
                                }
                            }
                            if(iCounter == 0) {
                                // first time count
                                threshold_ = min_;
                                activeSize = maxActive +1;
                                TRACE_VAR((double)max_);
                                TRACE_VAR((double)min_);
                            } else {
                                threshold_ += thresholdStep_;
                            }
                            TRACE_VAR((double)max_);
                            TRACE_VAR((double)min_);
                            TRACE_VAR(iCounter);
                            TRACE_VAR(activeSize);
                            TRACE_VAR((double)threshold_);
                            iCounter++;
                        }
                        TRACE_VAR(activeSize);
                        TRACE_VAR(maxActive);
                    }
                }
            }
            blobs4x4_.push_back(blob4x4);
        }
    }
#endif
}



/**
 * forward2
 * @return None.
 **/
void NewtonLayer::forward2(void)
{
    for(auto btm:bottom_){
        // 2x2
        auto input = dynamic_pointer_cast<LayerInput>(btm);
        auto inBlob_orig = input->getBlob(this);
        auto inBlob = inBlob_orig->grid(this->w_,this->h_);
        INFO_VAR(inBlob->w_);
        INFO_VAR(inBlob->h_);
        INFO_VAR(inBlob->ch_);
        this->wGrid2x2_ = inBlob->w_/this->w_;
        this->hGrid2x2_ = inBlob->h_/this->h_;
        INFO_VAR(this->wGrid2x2_);
        INFO_VAR(this->hGrid2x2_);
        auto raw2x2 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
        auto nextInput2x2 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));

        uint8_t max_2x2 = 0;
        uint8_t min_2x2 = 255;
        
        for(int i = 0 ;i < raw2x2->size_ ;i++) {
            uint8_t maxDiff = 0;
            uint8_t avg = 0;
            int index = i*this->w_*this->h_;
            cal4Vec(&(inBlob->data_[index]),maxDiff,avg);
            raw2x2->data_[i] = maxDiff;
            nextInput2x2->data_[i] = avg;
//            nextInput2x2->data_[i] = maxDiff;

            if(raw2x2->data_[i] > max_2x2) {
                max_2x2 = raw2x2->data_[i];
            }
            if(raw2x2->data_[i] < min_2x2) {
                min_2x2 = raw2x2->data_[i];
            }
        }
        blobsRaw2X2_.push_back(raw2x2);
        // 4x4
        this->wGrid4x4_ =this->wGrid2x2_/2;
        this->hGrid4x4_ =this->hGrid2x2_/2;
        INFO_VAR(this->wGrid4x4_);
        INFO_VAR(this->hGrid4x4_);
        auto swap4x4 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
        for(int channel = 0 ; channel < inBlob->ch_;channel++){
            for(int y = 0;y < this->hGrid2x2_;y++){
                for(int x = 0;x < this->wGrid2x2_;x++){
                    int indexInput = channel *(this->hGrid2x2_ * this->wGrid2x2_) + y*this->wGrid2x2_ + x;
                    auto byte = nextInput2x2->data_[indexInput];
                    
                    const int grid = ((y/2) * (wGrid2x2_/2))+ (x/2) ;
                    TRACE_VAR(grid);
                    const int groundW = (wGrid2x2_/2)*2;
                    const int groundH = (hGrid2x2_/2)*2;
                    if(groundW < x || groundH < y) {
                        // out side of last grid.
                        continue;
                    }
                    int index = channel * groundW * groundH;
                    index += grid * 2 * 2;
                    index += (y%2)*2  + x%2 ;
                    TRACE_VAR(index);
                    if(index > swap4x4->size_) {
                        INFO_VAR(index);
                        INFO_VAR(swap4x4->size_);
                        continue;
                    }
                    swap4x4->data_[index] = byte;
                }
            }
        }
        //
        auto raw4x4 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid4x4_,this->hGrid4x4_,inBlob->ch_));
        auto nextInput4x4 = shared_ptr<Blob<uint8_t>>(new Blob<uint8_t>(this->wGrid4x4_,this->hGrid4x4_,inBlob->ch_));
        uint8_t max_4x4 = 0;
        uint8_t min_4x4 = 255;
        for(int i = 0 ;i < raw4x4->size_ ;i++) {
            uint8_t maxDiff = 0;
            uint8_t avg = 0;
            int index = i*this->w_*this->h_;
            cal4Vec(&(swap4x4->data_[index]),maxDiff,avg);
            raw4x4->data_[i] = maxDiff;
            nextInput4x4->data_[i] = avg;

            if(raw4x4->data_[i] > max_4x4) {
                max_4x4 = raw4x4->data_[i];
            }
            if(raw4x4->data_[i] < min_4x4) {
                min_4x4 = raw4x4->data_[i];
            }
        }
        blobsRaw4X4_.push_back(raw4x4);
        
        for (auto top:top_) {
            auto blob2x2 = shared_ptr<Blob<bool>>(new Blob<bool>(this->wGrid2x2_,this->hGrid2x2_,inBlob->ch_));
            for (int ch = 0; ch < inBlob->ch_; ch++) {
                const int oneChannel = this->wGrid2x2_ * this->hGrid2x2_;
                const int maxActive = oneChannel * this->sparseFractions_ / sparseNumerator_;
                int activeSize = maxActive +1;
                uint8_t threshold_ = min_2x2;
                uint8_t thresholdStep_ = 1;
                while (activeSize > maxActive) {
                    activeSize = 0;
                    for(int i =0;i < oneChannel;i++)
                    {
                        int index = ch * oneChannel + i;
                        int delta = raw2x2->data_[index] - threshold_;
                        if(0 < delta ){
                            blob2x2->data_[index] = true;
                            activeSize++;
                        }else{
                            blob2x2->data_[index] = false;
                        }
                    }
                    threshold_ += thresholdStep_;
                }
            }
            blobs2x2_.push_back(blob2x2);
            
            auto blob4x4 = shared_ptr<Blob<bool>>(new Blob<bool>(this->wGrid4x4_,this->hGrid4x4_,inBlob->ch_));
            for (int ch = 0; ch < inBlob->ch_; ch++) {
                const int oneChannel = this->wGrid4x4_ * this->hGrid4x4_;
                const int maxActive = oneChannel * this->sparseFractions_ / sparseNumerator_;
                int activeSize = maxActive +1;
                uint8_t threshold_ = min_4x4;
                uint8_t thresholdStep_ = 1;
                while (activeSize > maxActive) {
                    activeSize = 0;
                    for(int i =0;i < oneChannel;i++)
                    {
                        int index = ch * oneChannel + i;
                        int delta = raw4x4->data_[index] - threshold_;
                        if(0 < delta ){
                            blob4x4->data_[index] = true;
                            activeSize++;
                        }else{
                            blob4x4->data_[index] = false;
                        }
                    }
                    threshold_ += thresholdStep_;
                }
            }
            blobs4x4_.push_back(blob4x4);
        }
    }
}


/**
 * cut point that is not connect to anthers.
 * @return None.
 **/
void NewtonLayer::cutIsolation(shared_ptr<Blob<bool>> blob)
{
#if 0 // do 3x3 filter.
    for (int ch = 0; ch < blob->ch_; ch++) {
        for (int x = 0;x < blob->w_;x++){
            for (int y = 0;y < blob->h_;y++){
                int index = ch * blob->w_ * blob->h_ + y*blob->w_ + x;
                
                int xL = x ;
                
                if(blob->data_[index]) {
                    
                }
            }
        }
    }
#endif
}


/**
 * cut point that is not connect to anthers.
 * @return None.
 **/
void NewtonLayer::cutIsolation(void)
{
    for (auto &blob:blobs2x2_) {
        cutIsolation(blob);
    }
    for (auto &blob:blobs4x4_) {
        cutIsolation(blob);
    }
}


/**
 * dump to png
 * @return None.
 **/
void NewtonLayer::dump(void){
    INFO_VAR(blobs2x2_.size());
    for (auto blob:blobs2x2_) {
        string name = typeid(this).name();
        name += "_2x2";
        blob->dump( name);
    }
#if 1
    for (auto blob:blobs4x4_) {
        string name = typeid(this).name();
        name += "_4x4";
        blob->dump( name);
    }
#endif
}

/**
 * get ptr
 * @return None.
 **/
shared_ptr<Blob<bool>> NewtonLayer::getBlob(const LayerBase* who)
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


/**
 * get ptr
 * @return None.
 **/
shared_ptr<Blob<bool>> NewtonLayer::getBlob2X2(const LayerBase* who)
{
    int i = 0;
    TRACE_VAR(top_.size());
    for(auto top:top_)
    {
        if(who == top.get())
        {
            TRACE_VAR(blobs2x2_.size());
            if(blobs2x2_.size() > i)
            {
                return blobs2x2_[i];
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

/**
 * get ptr
 * @return None.
 **/
shared_ptr<Blob<bool>> NewtonLayer::getBlob4X4(const LayerBase* who)
{
    int i = 0;
    TRACE_VAR(top_.size());
    for(auto top:top_)
    {
        if(who == top.get())
        {
            TRACE_VAR(blobs4x4_.size());
            if(blobs4x4_.size() > i)
            {
                return blobs4x4_[i];
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

