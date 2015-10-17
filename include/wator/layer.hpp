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
    class Blob;
    /**
     * BaseLayer Base of all type Net layers.
     **/ 
    class LayerBase {
    public:
        /**
         * forward
         * @return None.
         **/
        virtual void forward(void);
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
        shared_ptr<Blob> blob_;
        float threshold_;
        vector<float> activeWeight_;
        vector<float> deactiveWeight_;
        vector<float> weight_;
    };

    /**
     * LayerInput Base of all type Net layers.
     **/
    class LayerInput :public LayerBase{
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
        
    protected:
    };

    /**
     * LayerInput Base of all type Net layers.
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
