#include "point_iterator.hpp" // overwrite for temporary fix.
#include <iostream>
#include <fstream>
#include <config.hpp>
#include <chrono>
#include <boost/program_options.hpp>
#include "misc.hpp"
#include "syntax/parser.hpp"
#include "runtime/runner.hpp"
#include "server/server.hpp"
#include "tester/tester.hpp"
#include "compiler/compiler.hpp"

void repl(){
    using namespace shiranui;
    using namespace shiranui::syntax;
    using namespace shiranui::runtime;
    shiranui::runtime::Runner r;
    shiranui::syntax::ast::PrettyPrinterForAST printer(std::cerr);
    shiranui::runtime::value::PrettyPrinterForValue printer_for_value(std::cerr);
    std::cerr << "This is " << PACKAGE_STRING << std::endl;
    while(true){
        std::cout << "> ";
        std::string str;
        std::getline(std::cin,str);
        str += "\n";
        if(std::cin.eof()){
            std::cerr << "good bye shirei" << std::endl;
            return;
        }
        pos_iterator_t first(str.begin()),last(str.end());
        pos_iterator_t iter = first;
        sp<ast::SourceCode> program;
        bool ok = false;
        try{
            Parser<pos_iterator_t> resolver;
            ok = parse(iter,last,resolver,program);
        }catch (boost::spirit::qi::expectation_failure<pos_iterator_t> const& x){
            std::cerr << "expected: ";
            std::cerr << x.what_ << std::endl;
            std::cerr << "got: \"" << std::string(x.first, x.last) << '"' << std::endl;
            continue;
        }
        if(ok and iter == last){
            program->accept(printer);
            try{
                program->accept(r);
                r.cur_v->accept(printer_for_value);
                std::cerr << std::endl;
            }catch(NoSuchVariableException e){
                std::cerr << "No such variable: ";
                e.where->accept(printer);
                std::cerr << std::endl;
            }catch(ConvertException e){
                std::cerr << "Convert Error: ";
                e.where->accept(printer);
                std::cerr << std::endl;
            }catch(RuntimeException e){
                std::cerr << "Something RuntimeException: ";
                e.where->accept(printer);
                std::cerr << std::endl;
            }
        }else{
            std::cout << "-------------------------\n";
            std::cout << "ERROR: Parsing failed or not complete\n";
            std::cout << "remaining: '" << std::string(iter, last) << "'\n";
            std::cout << "-------------------------\n";
        }
    }
}
void start_server(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    shiranui::server::PipeServer server(std::cin,std::cout);
    server.start();
}

void exec(const std::string content){
    using namespace shiranui;
    using namespace shiranui::syntax;
    using namespace shiranui::runtime;
    shiranui::runtime::Runner r;
    shiranui::syntax::ast::PrettyPrinterForAST printer(std::cerr);
    shiranui::runtime::value::PrettyPrinterForValue printer_for_value(std::cerr);
    std::string str = content;
    pos_iterator_t first(str.begin()),last(str.end());
    pos_iterator_t iter = first;
    sp<ast::SourceCode> program;
    bool ok = false;
    try{
        Parser<pos_iterator_t> resolver;
        ok = parse(iter,last,resolver,program);
    }catch (boost::spirit::qi::expectation_failure<pos_iterator_t> const& x){
        std::cerr << "expected: ";
        std::cerr << x.what_ << std::endl;
        std::cerr << "got: \"" << std::string(x.first, x.last) << '"' << std::endl;
        return;
    }
    if(ok and iter == last){
        //program->accept(printer);
        try{
            program->accept(r);
            //r.cur.v->accept(printer_for_value);
            //std::cerr << std::endl;
        }catch(NoSuchVariableException e){
            std::cerr << "No such variable: ";
            e.where->accept(printer);
            std::cerr << std::endl;
        }catch(ConvertException e){
            std::cerr << "Convert Error: ";
            e.where->accept(printer);
            std::cerr << std::endl;
        }catch(RuntimeException e){
            std::cerr << "Something RuntimeException: ";
            e.where->accept(printer);
            std::cerr << std::endl;
        }
    }else{
        std::cout << "-------------------------\n";
        std::cout << "ERROR: Parsing failed or not complete\n";
        std::cout << "remaining: '" << std::string(iter, last) << "'\n";
        std::cout << "-------------------------\n";
    }
    return;
}

std::string compile_to_cpp(const std::string content){
    using namespace shiranui;
    using namespace shiranui::syntax;
    using namespace shiranui::runtime;
    using namespace shiranui::compiler;
    std::string str = content;
    pos_iterator_t first(str.begin()),last(str.end());
    pos_iterator_t iter = first;
    sp<ast::SourceCode> program;
    bool ok = false;
    try{
        Parser<pos_iterator_t> resolver;
        ok = parse(iter,last,resolver,program);
    }catch (boost::spirit::qi::expectation_failure<pos_iterator_t> const& x){
        std::cerr << "expected: ";
        std::cerr << x.what_ << std::endl;
        std::cerr << "got: \"" << std::string(x.first, x.last) << '"' << std::endl;
        return "error";
    }
    if(ok and iter == last){
        Compiler c;
        program->accept(c);
        std::string s = c.str();
        return s;
    }
    return "error";
}

int main(int argc,char **argv){
    namespace po = boost::program_options;
    po::options_description opt("No arguments options");
    opt.add_options()
        ("help,h","print help message")
        ("server,s","run Shiranui in server mode")
        ("kagero,k",po::value<std::string>(),"Specify Kagero(Standard) library path")
        ("exec,e",po::value<std::string>(),"execute given file")
        ("compile,c",po::value<std::string>(),"compile file to c++")
        ("arare,a",po::value<std::string>(),"Specify Arare(launchpad) path")
        ("out,o",po::value<std::string>(),"Specify output file")
        ("test,t","test(donot use.)")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,opt),vm);
    po::notify(vm);
    if (vm.count("help")){
        std::cout << opt << std::endl;
        return 1;
    }
    if(vm.count("server")){
        start_server();
        return 0;
    }
    if(vm.count("exec")){
        std::string filename = vm["exec"].as<std::string>();
        std::fstream fs(filename);
        std::string str((std::istreambuf_iterator<char>(fs)),
                         std::istreambuf_iterator<char>());
        if(vm.count("kagero")){
            std::string kagerofile = vm["kagero"].as<std::string>();
            std::fstream kagero_fs(kagerofile);
            std::string kagero_str((std::istreambuf_iterator<char>(kagero_fs)),
                                   (std::istreambuf_iterator<char>()));
            str = kagero_str + str;

        }
        exec(str);
        return 0;
    }
    if(vm.count("test")){
        shiranui::tester::run_test();
        return 0;
    }
    if(vm.count("compile")){
        std::string filename = vm["compile"].as<std::string>();
        std::fstream fs(filename);
        std::string str((std::istreambuf_iterator<char>(fs)),
                        std::istreambuf_iterator<char>());
        if(vm.count("kagero")){
            std::string kagerofile = vm["kagero"].as<std::string>();
            std::fstream kagero_fs(kagerofile);
            std::string kagero_str((std::istreambuf_iterator<char>(kagero_fs)),
                                   (std::istreambuf_iterator<char>()));
            str = kagero_str + str;
        }
        std::string compiled = compile_to_cpp(str);
        if(vm.count("arare")){
            std::string ararefile = vm["arare"].as<std::string>();
            std::fstream arare_fs(ararefile);
            std::string arare_str((std::istreambuf_iterator<char>(arare_fs)),
                                   (std::istreambuf_iterator<char>()));
            compiled = arare_str + compiled;
        }
        if(vm.count("out")){
            std::ofstream os(vm["out"].as<std::string>());
            os << compiled << std::endl;
        }else{
            std::cout << compiled << std::endl;
        }

        return 0;
    }
    // otherwise
    repl();
    return 0;
}
