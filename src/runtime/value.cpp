#include "value.hpp"
#include "runner.hpp"

namespace shiranui{
    namespace runtime{
        namespace value{
            Integer::Integer(int v) : value(v) {};
            void Integer::accept(VisitorForValue& v){
                v.visit(*this);
            }
            // String
            String::String(std::string v) : value(v) {}
            void String::accept(VisitorForValue& v){
                v.visit(*this);
            }
            Boolean::Boolean(bool v) : value(v) {}
            void Boolean::accept(VisitorForValue& v){
                v.visit(*this);
            }

            // Return
            Return::Return(Value* v) : value(v) {}
            Return::Return(sp<Value> v) : value(v) {}
            void Return::accept(VisitorForValue& v){
                v.visit(*this);
            }
            // Function
            UserFunction::UserFunction(std::vector<ast::Identifier> ps,
                                       sp<ast::Block> b,
                                       sp<environment::Environment> e)
                : body(b),env(e){
                    parameters = ps;
            }
            void UserFunction::accept(VisitorForValue& v){
                v.visit(*this);
            }

            SystemCall::SystemCall(){
                parameters = {ast::Identifier("str")};
            }
            void SystemCall::accept(VisitorForValue& v){
                v.visit(*this);
            }

            namespace builtin{
                PrintFunction::PrintFunction(){
                    parameters = {ast::Identifier("str")};
                }
                void PrintFunction::accept(VisitorForValue& v){
                    v.visit(*this);
                }
            }
        }
    }
}

namespace shiranui{
    namespace runtime{
        namespace value{
            PrettyPrinterForValue::PrettyPrinterForValue(std::ostream& os_)
                : os(os_){
                }
            void PrettyPrinterForValue::visit(Integer& i){
                os << i.value;
            }
            void PrettyPrinterForValue::visit(String& s){
                os << '"' << s.value << '"';
            }
            void PrettyPrinterForValue::visit(Boolean& b){
                os << (b.value?"true":"false");
            }
            void PrettyPrinterForValue::visit(UserFunction& f){
                using shiranui::syntax::ast::PrettyPrinterForAST;
                PrettyPrinterForAST printer_for_ast(os);
                os << "\\(";
                for(size_t i=0;i<f.parameters.size();i++){
                    f.parameters[i].accept(printer_for_ast);
                    if(i != f.parameters.size()-1){
                        os << ",";
                    }
                }
                os << ")" << std::endl;
                f.body->accept(printer_for_ast);
            }
            void PrettyPrinterForValue::visit(Return& r){
                os << "(return ";
                r.value->accept(*this);
                os << ")";
            }

            // builtin
            void PrettyPrinterForValue::visit(SystemCall& sc){
                os << "system_call";
            }

            void PrettyPrinterForValue::visit(builtin::PrintFunction& f){
                os << "print ";
            }
        }
    }
}
