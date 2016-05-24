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
#include <memory>
using namespace std;
#include <wator.hpp>
using namespace Wator;
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
namespace log = boost::log;


static void train(void){
    shared_ptr<ObjectLayer> object(new ObjectLayer);

    shared_ptr<V4CortexLayer> v4(new V4CortexLayer);
    object->bottom(v4);
    v4->top(object);

    shared_ptr<V3CortexLayer> v3(new V3CortexLayer);
    v4->bottom(v3);
    v3->top(v4);

    
    shared_ptr<V2CortexLayer> v2(new V2CortexLayer);
    v3->bottom(v2);
    v2->top(v3);

    shared_ptr<V1CortexLayer> v1(new V1CortexLayer);
    v2->bottom(v1);
    v1->top(v2);

#if 1
    shared_ptr<LeibnizLayer> leibn(new LeibnizLayer);
    v1->bottom(leibn);
    leibn->top(v1);

    shared_ptr<NewtonLayer> newton(new NewtonLayer);
    leibn->bottom(newton);
    newton->top(leibn);
#else
    shared_ptr<NewtonLayer> newton(new NewtonLayer);
    v1->bottom(newton);
    newton->top(v1);
#endif

    shared_ptr<ImageLayer> img(new ImageLayer);
    newton->bottom(img);
    img->top(newton);

    Net net;
    net << img;
    
    net.train();
}
static void test(void){
}

int main(int ac,char *av[]){
    
    log::core::get()->set_filter(log::trivial::severity >= log::trivial::trace);
//    log::core::get()->set_filter(log::trivial::severity >= log::trivial::debug);
    
    po::options_description opt("option");
    opt.add_options()
    ("help,h", "show help")
    ("phase,p", po::value<std::string>(), "phase test train")
    ("snapshot,s", po::value<std::string>(), "snapshot nn mode")
    ("configure,c", po::value<std::string>(), "net configure file ");
    
    po::variables_map vm;
    try {
        auto parser = po::command_line_parser(ac, av);
        po::store(parser.options(opt).run(), vm);
        po::notify(vm);
        
        if (vm.count("help")) {
            std::cout << opt << std::endl;
        }
        if (vm.count("configure")) {
            std::cout << vm["configure"].as<std::string>() << std::endl;
        }
        if (vm.count("phase")) {
            auto action = vm["phase"].as<std::string>();
            std::cout << action << std::endl;
            if("test"==action){
                test();
            }
            if("train"==action){
                train();
            }
        }
    } catch(const po::error_with_option_name& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
