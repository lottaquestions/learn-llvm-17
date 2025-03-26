#ifndef AST_H
# define AST_H
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
class AST;
class Expr;
class Factor;
class BinaryOp;
class WithDecl;
class ASTVisitor{
    virtual void visit(AST &){}
    virtual void visit(Expr &){}
    virtual void visit(Factor &) = 0;
    virtual void visit(BinaryOp &) = 0;
    virtual void visit(WithDecl &) = 0;
};

class AST{
    public:
    virtual ~AST(){}
    virtual void accept(ASTVisitor &V) = 0;
};

class Expr : public AST{
    public:
    Expr(){}
};

class Factor : public Expr{
    public:
    enum ValueKind {Ident, Number};
    private:
    ValueKind Kind;
    llvm::StringRef Val;
    public:
    Factor(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val){}
    ValueKind getKind() { return Kind; }
    llvm::StringRef getVal() { return Val; }
    virtual void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};

class BinaryOp : public Expr{
    public:
    enum Operator { Plus, Minus, Mul, Div };
    private:
    Expr *Left;
    Expr *Right;
    Operator Op;
    public:
    BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R){}
    Expr *getLeft() const { return Left; }
    Expr *getRight() const { return Right; }
    Operator getOperator() const { return Op; }
    virtual void accept(ASTVisitor &V) override {
        V.visit(*this);
    }
};

#endif