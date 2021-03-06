#ifndef COMPILER_HPP_INCLUDED
#define COMPILER_HPP_INCLUDED

#include "../syntax/ast.hpp"
#include <sstream>

namespace shiranui{
    namespace compiler{
        struct Compiler : syntax::ast::VisitorForAST{
            std::stringstream ss;
            Compiler();
            std::string str();
            void visit(syntax::ast::Identifier&);
            void visit(syntax::ast::Variable&);
            void visit(syntax::ast::Number&);
            void visit(syntax::ast::String&);
            void visit(syntax::ast::Enum&);
            void visit(syntax::ast::Interval&);
            void visit(syntax::ast::Block&);
            void visit(syntax::ast::Function&);
            void visit(syntax::ast::FunctionCall&);
            void visit(syntax::ast::BinaryOperator&);
            void visit(syntax::ast::UnaryOperator&);
            void visit(syntax::ast::IfElseExpression&);
            void visit(syntax::ast::Definement&);
            void visit(syntax::ast::ReturnStatement&);
            void visit(syntax::ast::AssertStatement&);
            void visit(syntax::ast::IfElseStatement&);
            void visit(syntax::ast::ForStatement&);
            void visit(syntax::ast::Assignment&);
            void visit(syntax::ast::TestFlyLine&);
            void visit(syntax::ast::IdleFlyLine&);
            void visit(syntax::ast::SourceCode&);
        };
    }
}
#endif
