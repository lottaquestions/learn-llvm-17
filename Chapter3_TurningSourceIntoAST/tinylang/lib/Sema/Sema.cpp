#include "tinylang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

using namespace tinylang;

void Sema::enterScope(Decl *D) {
    CurrentScope = new Scope(CurrentScope);
    CurrentDecl = D;
}

void Sema::leaveScope(){
    assert(CurrentScope && "Can't leave non-existing scope");
    Scope *Parent = CurrentScope->getParent();
    delete CurrentScope;
    CurrentScope = Parent;
    CurrentDecl = CurrentDecl->getEnclosingDecl();
}

bool Sema::isOperatorForType(tok::TokenKind Op, TypeDeclaration *Ty){
    switch (Op)
    {
    case tok::plus:
    case tok::minus:
    case tok::star:
    case tok::kw_DIV:
    case tok::kw_MOD:
        return Ty == IntergerType;
    case tok::slash:
        return false; // REAL not implemented
    case tok::kw_AND:
    case tok::kw_OR:
    case tok::kw_NOT:
        return Ty == BooleanType;
    
    default:
        llvm_unreachable("Unknown operator");
    }
}

void Sema::checkFormalAndActualParameters(SMLoc Loc, const FormalParamList &Formals, const ExprList &Actuals){
    if(Formals.size() != Actuals.size()) {
        Diags.report(Loc, diag::err_wrong_number_of_parameters);
        return;
    }

    auto A = Actuals.begin();
    for(auto I = Formals.begin(), E = Formals.end(); I != E; ++I, ++A){
        FormalParameterDeclaration *F = *I;
        Expr *Arg = *A;
        if(F->getType() != Arg->getType()){
            Diags.report(Loc, diag::err_type_of_formal_and_actual_parameter_not_compatible);
        }
        if(F->isVar() && isa<VariableAcccess>(Arg)){ // TODO: Confirm that the second check is correct i.e. I was expecting !isa<...
            Diags.report(Loc, diag::err_var_parameter_requires_var);
        }
    }
}

void Sema::initialize(){
    // Setup global scope
    CurrentScope = new Scope();
    CurrentDecl = nullptr;
    IntergerType = new TypeDeclaration(CurrentDecl, SMLoc(), "INTEGER");
    BooleanType = new TypeDeclaration(CurrentDecl, SMLoc(), "BOOLEAN");
    TrueLiteral = new BooleanLiteral(true, BooleanType);
    FalseLiteral = new BooleanLiteral(false, BooleanType);
    TrueConst = new ConstantDeclaration(CurrentDecl, SMLoc(), "TRUE", TrueLiteral);
    FalseConst = new ConstantDeclaration(CurrentDecl, SMLoc(), "FALSE", FalseLiteral);
    CurrentScope->insert(IntergerType);
    CurrentScope->insert(BooleanType);
    CurrentScope->insert(TrueConst);
    CurrentScope->insert(FalseConst);
}