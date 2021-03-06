#include "compiler.hpp"

namespace shiranui{
    namespace compiler{
        using namespace shiranui::syntax::ast;
        Compiler::Compiler(){
        }
        std::string Compiler::str(){
            return ss.str();
        }
        void Compiler::visit(Identifier& node){
            ss << node.name;
            // throw InternalException(std::make_shared<Identifier>(node));
        }
        void Compiler::visit(Variable& node){
            ss << node.value.name;
        }
        void Compiler::visit(Number& node){
            ss << node.value;
        }
        void Compiler::visit(String& node){
            ss << '"' << node.value << '"';
        }
        void Compiler::visit(Enum& node){
            ss << "{";
            for(sp<Expression> e : node.expressions){
                e->accept(*this);
                ss << ",";
            }
            ss << "}";
        }
        void Compiler::visit(Interval& node){
            ss << "arare::range(";
            node.start->accept(*this);
            ss << ",";
            if(node.next != nullptr){
                node.next->accept(*this);
                ss << ",";
            }
            node.end->accept(*this);
            ss << ",";
            ss << (node.right_close?"true":"false");
            ss << ")";
        }
        void Compiler::visit(Block& node){
            ss << "{" << std::endl;
            for(sp<Statement> s : node.statements){
                s->accept(*this);
                ss << ";" << std::endl;
            }
            ss << "}" << std::endl;
        }
        void Compiler::visit(Function& node){
            ss << "[&]" << "(";
            for(size_t i=0;i<node.parameters.size();i++){
                ss << "auto ";
                node.parameters[i].accept(*this);
                if(i != node.parameters.size()-1){
                    ss << ",";
                }
            }
            ss << ")";
            node.body->accept(*this);
        }
        void Compiler::visit(FunctionCall& node){
            node.function->accept(*this);
            ss << "(";
            for(size_t i=0;i<node.arguments.size();i++){
                node.arguments[i]->accept(*this);
                if(i != node.arguments.size()-1){
                    ss << ",";
                }
            }
            ss << ")";
        }
        void Compiler::visit(BinaryOperator& node){
            ss << "(";
            node.left->accept(*this);
            if(node.op == "="){
                ss << "==";
            }else if(node.op == "/="){
                ss << "!=";
            }else{
                ss << node.op;
            }
            node.right->accept(*this);
            ss << ")";
        }
        void Compiler::visit(UnaryOperator& node){
            ss << node.op;
            node.exp->accept(*this);
        }
        void Compiler::visit(IfElseExpression& node){
            node.pred->accept(*this);
            node.ife->accept(*this);
            node.elsee->accept(*this);
        }
        void Compiler::visit(Definement& node){
            sp<Function> f = std::dynamic_pointer_cast<Function>(node.value);
            bool is_function = f != nullptr;
            if(node.id.name == "main"){
                if(is_function){
                    ss << "int main()" << std::endl;
                    f->body->accept(*this);
                }else{
                }
            }else{
                if(is_function){
                    if(f->parameters.size() != 0){
                        ss << "template<";
                        char type = 'T';
                        for(size_t i=0;i<f->parameters.size();i++){
                            ss << "typename " << (char)(type+i);
                            if(i != f->parameters.size()-1){
                                ss << ",";
                            }
                        }
                        ss << ">";
                        ss << std::endl;
                        ss << "auto " << node.id.name << "(";
                        for(size_t i=0;i<f->parameters.size();i++){
                            ss << (char)(type+i) << " ";
                            f->parameters[i].accept(*this);
                            if(i != f->parameters.size()-1){
                                ss << ",";
                            }
                        }
                        ss << ") -> decltype(auto)";
                        f->body->accept(*this);
                    }
                }else{
                    if(node.is_const){
                        ss << "const ";
                    }
                    ss << "auto ";
                    node.id.accept(*this);
                    ss << " = ";
                    node.value->accept(*this);
                }
            }
        }
        void Compiler::visit(ReturnStatement& node){
            ss << "return ";
            node.value->accept(*this);
        }
        void Compiler::visit(AssertStatement& node){
            ss << "assert(";
            node.value->accept(*this);
            ss << ")";
        }
        void Compiler::visit(IfElseStatement& node){
            ss << "if(";
            node.pred->accept(*this);
            ss << ")";
            node.ifblock->accept(*this);
            ss << "else";
            node.elseblock->accept(*this);
        }
        // should return forstatement?
        void Compiler::visit(ForStatement& node){
            ss << "for(auto ";
            node.loop_var.accept(*this);
            ss << ": ";
            node.loop_exp->accept(*this);
            ss << ")";
            node.block->accept(*this);
        }
        void Compiler::visit(Assignment& node){
            node.id.accept(*this);
            ss << " = ";
            node.value->accept(*this);
        }
        void Compiler::visit(TestFlyLine& node){
            ss << "//#- ";
            node.left->accept(*this);
            ss << " -> ";
            node.right->accept(*this);
            if(node.error != nullptr){
                ss << " || ";
                node.error->accept(*this);
            }
        }
        void Compiler::visit(IdleFlyLine& node){
            ss << "//#+ ";
            node.left->accept(*this);
            ss << " -> ";
            if(node.right != nullptr){
                node.right->accept(*this);
            }
        }
        void Compiler::visit(SourceCode& node){
            // merge statements and flylines.
            for(size_t si=0,fi=0;si<node.statements.size() or
                              fi<node.flylines.size();){
                bool statement_first = false;
                if(si != node.statements.size() and
                   fi != node.flylines.size() and
                   node.statements[si]->point < node.flylines[fi]->point){
                    statement_first = true;
                }else if(fi == node.flylines.size()){
                    statement_first = true;
                }

                if(statement_first){
                    node.statements[si]->accept(*this);
                    ss << ";" << std::endl;
                    si++;
                }else{
                    node.flylines[fi]->accept(*this);
                    ss << std::endl;
                    fi++;
                }
            }
        }
    }
}
