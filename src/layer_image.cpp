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

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>

#define DEBUG_VAR(x) {BOOST_LOG_TRIVIAL(debug) << #x << "=<" << x << ">" <<endl;}


#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
namespace fs = boost::filesystem;

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>


/**
 * Constructor
 **/
ImageLayer::ImageLayer()
{
}
/**
 * forward
 * @return None.
 **/
void ImageLayer::forward(void)
{
    DEBUG_VAR(param_.root_);
    const fs::path path(param_.root_);
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path),fs::recursive_directory_iterator())){
        if (!fs::is_directory(p)){
            DEBUG_VAR(p.extension().string());
            if(".jpg" == p.extension().string() || ".png" == p.extension().string() ||
               ".JPG" == p.extension().string() || ".PNG" == p.extension().string()){
                DEBUG_VAR(p);
                mat_ = cv::imread(p.string());
            }
        }
    }
}
