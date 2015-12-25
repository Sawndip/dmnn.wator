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

/**
 * Constructor
 **/
V1CortexLayer::V1CortexLayer()
{
}





/**
 * update
 * @return None.
 **/
void V1CortexLayer::update(void)
{
    INFO_VAR("finnish V1CortexLayer::update");
}




/**
 * round
 * @return None.
 **/
void V1CortexLayer::round(void)
{
    this->forward();
    this->update();
    INFO_VAR("finnish V1CortexLayer::round");
    LayerBase::round();
    this->dump();
}


/**
 * forward
 * @return None.
 **/
void V1CortexLayer::forward(void)
{
    filter_.clear();
    blobs_.clear();
    for(auto btm:bottom_){
        CoulombLayer *coulom  = dynamic_cast<CoulombLayer*>(btm);
        auto coulomBlob = coulom->getBlob(this);
        INFO_VAR(coulomBlob->w_);
        INFO_VAR(coulomBlob->h_);
        INFO_VAR(coulomBlob->ch_);
        const int size = (coulomBlob->w_/this->w_ )* (coulomBlob->h_ /this->h_)* coulomBlob->ch_;
        for (auto top:top_) {
            for (int ch = 0; ch < coulomBlob->ch_; ch++) {
                ;
                for (int x = 0; x < coulomBlob->w_; x++) {
                    ;
                    for (int y = 0; y < coulomBlob->h_; y++) {
                        ;
                    }
                }
            }
        }
    }
    INFO_VAR(blobs_.size());
    INFO_VAR("finnish V1CortexLayer::forward");
}


/**
 * dump to png
 * @return None.
 **/
void V1CortexLayer::dump(void){
    for (auto blob:blobs_) {
        blob->dump();
    }
}
