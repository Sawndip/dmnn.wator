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
CoulombLayer::CoulombLayer()
{
    for(int y = 0 ;y < h_;y++){
        for(int x =0;x < w_ ;x++){
            // 电荷中心，在几何中心。高斯分布,中心亮，外部暗。
            float w = (x- w_/2) * (x- w_/2) + (y- h_/2) * (y- h_/2) +1;
//            float w = std::abs(x- w_/2) + std::abs(y- h_/2) +1;
            w = 1/w;
            TRACE_VAR(x);
            TRACE_VAR(y);
            weight_.push_back(w);
            TRACE_VAR(w);
        }
    }
    
    /// weight bias cal. 调整截距，使相同亮度的输出为0
    float sum = 1.0;
    bool minus = true;
    bool shock = false;
    float fConstCoulombStepOne = fConstCoulombDiff * weight_.size();
    float fConstCoulombStepTwo = fConstCoulombDiff / weight_.size();
    
    do {
        auto rate = fConstCoulombStepOne;
        if(shock) {
            rate = fConstCoulombStepTwo;
        }
        sum = 0.0;
        for (int i = 0;i < weight_.size();i++) {
            if (minus) {
                weight_[i] -= rate;
            } else {
                weight_[i] += rate;
            }
            
            sum += weight_[i];
        }
        TRACE_VAR(sum);
        if( sum - fConstCoulombDiff < 0) {
            minus = false;
        } else {
            if (false == minus) {
                shock = true;
            }
            minus = true;
        }
        TRACE_VAR(minus);
    } while (std::abs(sum) > fConstCoulombDiff);
    
    
    for (int i = 0;i < weight_.size();i++) {
        INFO_VAR(weight_[i]);
    }
    INFO_VAR(sum);
    INFO_VAR(weight_.size());

/*
 not use it.
    for(int y = 0 ;y < h_;y++){
        for(int x =0;x < w_ ;x++){
            // 电荷中心，在几何中心。高斯分布,中心暗，外部亮。
            float w = (x- w_/2) * (x- w_/2) + (y- h_/2) * (y- h_/2) +1;
            w = 1- 1/w;
            TRACE_VAR(x);
            TRACE_VAR(y);
            weight_R_.push_back(w);
            TRACE_VAR(w);
        }
    }

    /// weight bias cal. 调整截距，使相同亮度的输出为0
    fConstCoulombStepOne = fConstCoulombDiff * weight_R_.size();
    fConstCoulombStepTwo = fConstCoulombDiff / weight_R_.size();
    sum = 1.0;
    
    do {
        auto rate = fConstCoulombStepOne;
        if(shock) {
            rate = fConstCoulombStepTwo;
        }
        sum = 0.0;
        for (int i = 0;i < weight_R_.size();i++) {
            if (minus) {
                weight_R_[i] -= rate;
            } else {
                weight_R_[i] += rate;
            }
            
            sum += weight_R_[i];
        }
        TRACE_VAR(sum);
        if( sum - fConstCoulombDiff < 0) {
            minus = false;
        } else {
            if (false == minus) {
                shock = true;
            }
            minus = true;
        }
        TRACE_VAR(minus);
    } while (std::abs(sum) > fConstCoulombDiff);

    for (int i = 0;i < weight_R_.size();i++) {
        INFO_VAR(weight_R_[i]);
    }
    INFO_VAR(sum);
    INFO_VAR(weight_R_.size());
 */
}

/**
 * Connect a Layer to Net.
 * @param [in] layer
 **/
V1CortexLayer& CoulombLayer::operator << (V1CortexLayer &layer)
{
    this->top_.push_back(&layer);
    layer.bottom(this);
    INFO_VAR(top_.size());
    return layer;
}


/**
 * update
 * @return None.
 **/
void CoulombLayer::updateW(void){
}


/**
 * update
 * @return None.
 **/
void CoulombLayer::update(void)
{
    INFO_VAR("finnish CoulombLayer::update");
}


/**
 * aspect
 * @return None.
 **/
void CoulombLayer::aspect(int w,int h,int ch)
{
    this->wGrid_ = w;
    this->hGrid_ = h;
    this->chGrid_ = ch;
}


/**
 * round
 * @return None.
 **/
void CoulombLayer::round(void)
{
    this->forward();
    this->update();
    INFO_VAR("finnish CoulombLayer::round");
    LayerBase::round();
    this->dump();
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
 * forward
 * @return None.
 **/
void CoulombLayer::forward(void)
{
    blobsRaw_.clear();
    blobs_.clear();
    for(auto btm:bottom_){
        LayerInput *input = dynamic_cast<LayerInput*>(btm);
        auto inBlob = input->getBlob(this);
        INFO_VAR(inBlob->w_);
        INFO_VAR(inBlob->h_);
        INFO_VAR(inBlob->ch_);
        INFO_VAR(weight_.size());
        this->wGrid_ = inBlob->w_/this->w_;
        this->hGrid_ = inBlob->h_/this->h_;
        this->chGrid_ = inBlob->ch_;
        INFO_VAR(this->wGrid_);
        INFO_VAR(this->hGrid_);
        auto raw = shared_ptr<Blob<float>>(new Blob<float>(this->wGrid_,this->hGrid_,this->chGrid_));
        max_ = 0.0;
        min_ = 255.0 *255.0;
        for(int i = 0 ;i < raw->size_ ;i++) {
            float sum =0;
            float minLocal = 255.0 *255.0;
            float maxLocal = 0.0;
            float sumBase = 0.0;
            for(int j = 0;j<weight_.size();j++) {
                int index = i*weight_.size() + j;
                int data = inBlob->data_[index];
                sum += weight_[j] * (float)(data);
                sumBase += data;
                if(data > maxLocal){
                    maxLocal = data;
                }
                if(data < minLocal) {
                    minLocal = data;
                }
            }
            if(sum > max_) {
                max_ = sum;
            }
            if(sum < min_) {
                min_ = sum;
            }
            float rate1 = maxLocal-minLocal;
            float rate2 = 0;
            if(sumBase >0) {
                rate2 = 255.0/sumBase;
            }
            raw->data_[i] = sum * rate1 * rate2;
        }
        blobsRaw_.push_back(raw);
        
        
        /// cal every blob for every top
        for (auto top:top_) {
            auto blob = shared_ptr<Blob<bool>>(new Blob<bool>(this->wGrid_,this->hGrid_,this->chGrid_));
            V1CortexLayer *v1 = dynamic_cast<V1CortexLayer*>(top);
            auto gridW = this->wGrid_/v1->iW_;
            auto gridH = this->hGrid_/v1->iH_;
            for (int ch = 0; ch <this->chGrid_;ch++) {
                for (int x = 0;x < gridW;x++){
                    for (int y = 0;y < gridH;y++){
                        int activeSize = v1->iSparse_ + 1;
                        TRACE_VAR(activeSize);
                        const int maxActive = v1->iSparse_;
                        int iCounter = 0;
                        while (activeSize > maxActive) {
                            max_ = 0.0;
                            min_ = 255.0 *255.0;
                            activeSize = 0;
                            for(int x2 = 0 ;x2 < v1->iW_ ;x2++) {
                                for(int y2 = 0 ;y2 < v1->iH_ ;y2++) {
                                    /* index */
                                    auto index = ch * this->wGrid_* this->hGrid_ + (y * v1->iH_+y2) * this->wGrid_ + x*v1->iW_ + x2;
                                    if(iCounter == 0) {
                                    //if(0) {
                                        if(raw->data_[index] > max_) {
                                            max_ = raw->data_[index];
                                        }
                                        if(raw->data_[index] < min_) {
                                            min_ = raw->data_[index];
                                        }
                                    }
                                    float delta = std::abs(raw->data_[index] - thresholdCenter_) - threshold_;
                                    TRACE_VAR(delta);
                                    TRACE_VAR(raw->data_[index])
                                    if(0 < delta ){
                                        blob->data_[index] = true;
                                        activeSize++;
                                    }else{
                                        blob->data_[index] = false;
                                    }
                                }
                            }
                            if(iCounter == 0) {
                                // first time count
                                threshold_ = std::abs(0.8*(max_ - min_)/2);
                                thresholdCenter_ = (max_ + min_)/2;
                                activeSize = maxActive + 1;
                                TRACE_VAR(thresholdCenter_);
                                TRACE_VAR(max_);
                                TRACE_VAR(min_);
                            } else {
                                threshold_ += thresholdStep_;
                            }
                            TRACE_VAR(iCounter);
                            TRACE_VAR(activeSize);
                            TRACE_VAR(threshold_);
                            iCounter++;
                        }
                        TRACE_VAR(activeSize);
                        TRACE_VAR(maxActive);
                    }
                }
            }
            blobs_.push_back(blob);
        }
    }
    INFO_VAR(blobs_.size());
    INFO_VAR("finnish CoulombLayer::forward");
}


/**
 * dump to png
 * @return None.
 **/
void CoulombLayer::dump(void){
    INFO_VAR(blobs_.size());
    for (auto blob:blobs_) {
        blob->dump(typeid(this).name());
    }
}

/**
 * get ptr
 * @return None.
 **/
shared_ptr<Blob<bool>> CoulombLayer::getBlob(const LayerBase* who)
{
    int i = 0;
    TRACE_VAR(top_.size());
    for(auto top:top_)
    {
        if(who == top)
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
