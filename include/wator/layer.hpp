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
#include <memory>
#include <vector>
using namespace std;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;

namespace Wator {
    /**
     * BaseLayer Base of all type Net layers.
     **/ 
    class LayerBase {
        friend class ImageLayer;
    public:
        /**
         * load
         * @return None.
         **/
        virtual void load(void);
        
        /**
         * forward
         * @return None.
         **/
        virtual void forward(void);
        
        /**
         * top
         * @param [in] lyr layer.
         * @return None.
         **/
        void top(shared_ptr<LayerBase> lyr){this->top_.push_back(lyr);}

        /**
         * bottom
         * @param [in] lyr layer.
         * @return None.
         **/
        void bottom(shared_ptr<LayerBase> lyr){this->bottom_.push_back(lyr);}
    protected:
        /**
         * Constructor 
        **/
        explicit LayerBase();

        /**
         * Constructor 
         * @param [in] info layer parameter.
        **/
        explicit LayerBase(const ptree& info);

    protected:
        vector<shared_ptr<LayerBase> > bottom_;
        vector<shared_ptr<LayerBase> > top_;
        int threshold_;
        vector<int> activeWeight_;
        vector<int> deactiveWeight_;
        vector<int> weight_;
    };

    /**
     * LayerInput Base of Input layers.
     **/
    class LayerInput :public LayerBase{
        friend class Polarizer1stLayer;
    public:
    protected:
        /**
         * Constructor
         **/
        explicit LayerInput();
        
        /**
         * Constructor
         * @param [in] info layer parameter.
         **/
        explicit LayerInput(const ptree& info);
        
        /**
         * load
         * @return None.
         **/
        virtual void load(void);
        
        /**
         * get ptr
         * @return None.
         **/
        shared_ptr<Blob<uint16_t>> getBlob(const LayerBase* who);
    protected:
        vector<shared_ptr<Blob<uint16_t>>> blobs_;
    };

    /**
     * LayerOutput Base of Output layers.
     **/
    class LayerOutput :public LayerBase{
    public:
    protected:
        /**
         * Constructor
         **/
        explicit LayerOutput();
        
        /**
         * Constructor
         * @param [in] info layer parameter.
         **/
        explicit LayerOutput(const ptree& info);
        
    protected:
    };
    
}
