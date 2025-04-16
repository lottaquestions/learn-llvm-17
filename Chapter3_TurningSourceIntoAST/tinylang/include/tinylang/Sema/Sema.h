#ifndef TINYLANG_SEMA_SEMA_H
#define TINYLANG_SEMA_SEMA_H
#include "tinylang/AST/AST.h"
#include "tinylang/Basic/Diagnostic.h"
#include "tinylang/Sema/Scope.h"
#include <memory>

namespace tinylang{
    // TODO: complete
    class Sema
    {
    private:
        friend class EnterDeclScope;
        void enterScope(Decl *);
        void leaveScope();

        bool isOperatorForType(tok::TokenKind Op, TypeDeclaration *Ty);

        void checkFormalAndActualParameters(SMLoc Loc, const FormalParamList &Formals, const ExprList &Actuals);

        Scope *CurrentScope;
        Decl *CurrentDecl;
        DiagnosticsEngine &Diags;

        TypeDeclaration *IntergerType;
        TypeDeclaration *BooleanType;
        BooleanLiteral *TrueLiteral;
        BooleanLiteral *FalseLiteral;
        ConstantDeclaration *TrueConst;
        ConstantDeclaration *FalseConst;

    public:
        Sema(DiagnosticsEngine &Diags)
            : CurrentScope(nullptr), CurrentDecl(nullptr), Diags(Diags) {
            initialize();
        }

        void initialize();
        ModuleDeclaration *actOnModuleDeclaration(SMLoc Loc, StringRef Name);
        void actOnModuleDeclaration(ModuleDeclaration *ModDecl, SMLoc Loc, StringRef Name, DeclList &Decls, StmtList &Stmts);
        void actOnImport(StringRef ModuleName, IdentList &Ids);
        void actOnConstantDeclaration(DeclList &Decls, SMLoc Loc, StringRef Name, Expr *E);
        void actOnVariableDeclaration(DeclList &Decls, IdentList &Ids, Decl *D);
    };
    
} // namespace tinylang

#endif