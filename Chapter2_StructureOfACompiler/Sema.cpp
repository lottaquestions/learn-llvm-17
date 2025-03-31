#include "Sema.h"
#include "llvm/ADT/StringSet.h"
// The coding guidelines from LLVM forbid the use of the <iostream> library, 
// therefore, the header of the equivalent LLVM functionality is included
// e.g. llvm::errs() is defined in this file
#include "llvm/Support/raw_ostream.h"
namespace{
    class DeclCheck : public ASTVisitor {
        llvm::StringSet<> Scope;
        bool HasError;
        enum ErrorType { Twice, Not };
        void error(ErrorType ET, llvm::StringRef V){
            llvm::errs() << "Variable " << V << " "
            << (ET == Twice ? "already" : "not")
            << " declared\n";
            HasError = true;
        }
        public:
        DeclCheck() : HasError(false){}
        bool hasError() { return HasError; }

        // The names were previously stored in a set called Scope by the WithDecl
        // visitor. On a Factor node that holds a variable name, it is checked 
        // that the variable name is in the set. This catches variable use without
        // declaration cases.
        virtual void visit(Factor &Node) override {
            if(Node.getKind() == Factor::Ident){
                if(Scope.find(Node.getVal()) == Scope.end())
                error(Not, Node.getVal());
            }
        }

        // Check that both sides of a binary op exist and that they can be
        // successfully visited.
        virtual void visit(BinaryOp &Node) override {
            if(Node.getLeft()){
                Node.getLeft()->accept(*this);
            } else {
                HasError = true;
            }
            if(Node.getRight()){
                Node.getRight()->accept(*this);
            } else {
                HasError = true;
            }
        }

        // Populate the set of variable names, and then confirm that an
        // expression exists and that it can be successfully visited.
        virtual void visit(WithDecl &Node) override {
            for (auto I = Node.begin(), E = Node.end() ; I != E; ++I){
                if(!Scope.insert(*I).second)
                error(Twice, *I);
            }
            if(Node.getExpr()){
                Node.getExpr()->accept(*this);
            } else {
                HasError = true;
            }
            
        }
    };
    
} // namespace unnamed

bool Sema::semantic(AST *Tree){
    if(!Tree){
        return false;
    }
    DeclCheck Check;
    Tree->accept(Check);
    return Check.hasError();
}
