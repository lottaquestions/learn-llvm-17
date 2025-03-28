#include "Sema.h"
#include "llvm/ADT/StringSet.h"
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

        // The names are stored in a set called Scope. On a Factor node 
        // that holds a variable name, it is checked that the variable 
        // name is in the set
        virtual void visit(Factor &Node) override {
            if(Node.getKind() == Factor::Ident){
                if(Scope.find(Node.getVal()) == Scope.end())
                error(Not, Node.getVal());
            }
        }

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
