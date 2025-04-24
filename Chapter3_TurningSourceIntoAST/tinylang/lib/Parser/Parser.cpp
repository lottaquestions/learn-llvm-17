#include "tinylang/Parser/Parser.h"
#include "tinylang/Basic/TokenKinds.h"

using namespace tinylang;

namespace 
{
    OperatorInfo fromTok(Token Tok){
        return OperatorInfo(Tok.getLocation(), Tok.getKind());
    }
} // namespace 

Parser::Parser(Lexer &Lex, Sema &Actions) : Lex(Lex), Actions(Actions) {
    advance();
}

ModuleDeclaration *Parser::parse() {
    ModuleDeclaration *ModDecl = nullptr;
    parseCompilationUnit(ModDecl);
    return ModDecl;
}

bool Parser::parseCompilationUnit(ModuleDeclaration *&D) {
    auto _errorhandler = [this] { return skipUntil(); };
    if (consume(tok::kw_MODULE)){
        return _errorhandler();
    }
    if (expect(tok::identifier)){
        return _errorhandler();
    }
    D = Actions.actOnModuleDeclaration(Tok.getLocation(), Tok.getIdentifier());

    EnterDeclScope S(Actions, D);
    advance();
    if (consume(tok::semi)) {
        return _errorhandler();
    }
    while (Tok.isOneOf(tok::kw_FROM,  tok::kw_IMPORT)){
        if (parseImport()){
            return _errorhandler();
        }
    }
    DeclList Decls;
    StmtList Stmts;
    if (parseBlock(Decls, Stmts)) {
        return _errorhandler();
    }
    if (expect(tok::identifier)){
        return _errorhandler();
    }
    Actions.actOnModuleDeclaration(D, Tok.getLocation(), Tok.getIdentifier(), Decls, Stmts);
    advance();
    if (consume(tok::period)){
        return _errorhandler();
    }
    return false;
}

bool Parser::parseImport() {
    auto _errorhandler = [this] {
        // At import level, skip either to the next import or to the next declaration or block
        return skipUntil(tok::kw_BEGIN, tok::kw_CONST, tok::kw_END, tok::kw_FROM, tok::kw_IMPORT, tok::kw_PROCEDURE, tok::kw_VAR);
    };
    IdentList Ids;
    StringRef ModuleName;
    if (Tok.is(tok::kw_FROM)) {
        advance();
        if (expect(tok::identifier)) {
            return _errorhandler();
        }
        ModuleName = Tok.getIdentifier();
        advance();
    }
    if (consume(tok::kw_IMPORT)) {
        return _errorhandler();
    }
    if (parseIdentList(Ids)){
        return _errorhandler();
    }
    if (expect(tok::semi)){
        return _errorhandler();
    }
    Actions.actOnImport(ModuleName, Ids);
    advance();
    return false;
}

bool Parser::parseBlock(DeclList &Decls, StmtList &Stmts) {
    auto _errorhandler = [this] {
        // By resuming at the next identifier, you are likely to resume at the next valid
        // statement and not skip to far ahead.
        return skipUntil(tok::identifier);
    };
    while (Tok.isOneOf(tok::kw_CONST, tok::kw_PROCEDURE, tok::kw_VAR)) {
        if(parseDeclaration(Decls)){
            return _errorhandler();
        }
    }
    if (Tok.is(tok::kw_BEGIN)) {
        advance();
        if(parseStatementSequence(Stmts)) {
            return _errorhandler();
        }
    }
    if (consume(tok::kw_END)) {
        return _errorhandler();
    }
    return false;
}

bool Parser::parseDeclaration(DeclList &Decls) {
    auto _errorhandler = [this] {
        return skipUntil(tok::kw_BEGIN, tok::kw_CONST, tok::kw_END, tok::kw_PROCEDURE, tok::kw_VAR);
    }; 
    if (Tok.is(tok::kw_CONST)) {
        advance();
        while (Tok.is(tok::identifier)) {
            if (parseConstantDeclaration(Decls)) {
                return _errorhandler();
            }
            if (consume(tok::semi)) {
                return _errorhandler();
            }
        }
    } else if (Tok.is(tok::kw_VAR)){
        advance();
        while (Tok.is(tok::identifier)) {
            if (parseVariableDeclaration(Decls)) {
                return _errorhandler();
            }
            if (consume(tok::semi)) {
                return _errorhandler();
            }
        }
    } else if (Tok.is(tok::kw_PROCEDURE)) {
        if (parseProcedureDeclaration(Decls)) {
            return _errorhandler();
        }
        if (consume(tok::semi)) {
            return _errorhandler();
        }
    } else {
        // ERROR
        return _errorhandler();
    }
    return false;
}

bool Parser::parseConstantDeclaration(DeclList &Decls) {
    auto _errorhandler = [this] {
        return skipUntil(tok::semi);
    };
    if (expect(tok::identifier)) {
        return _errorhandler();
    }
    SMLoc Loc = Tok.getLocation();

    StringRef Name = Tok.getName();
    advance();
    if (expect(tok::equal)) {
        return _errorhandler();
    }
    Expr *E = nullptr;
    advance();
    if (parseExpression(E)) {
        return _errorhandler();
    }
    Actions.actOnConstantDeclaration(Decls, Loc, Name, E);
    return false;
}

bool Parser::parseVariableDeclaration(DeclList &Decls) {
    auto _errorhandler = [this] {
        return skipUntil(tok::semi);
    };
    Decl *D;
    IdentList Ids;
    if (parseIdentList(Ids)) {
        return _errorhandler();
    }
    if (consume(tok::colon)) {
        return _errorhandler();
    }
    if (parseQualident(D)) {
        return _errorhandler();
    }
    Actions.actOnVariableDeclaration(Decls, Ids, D);
    return false;
}

bool Parser::parseProcedureDeclaration(DeclList &ParentDecls) {
    auto _errorhandler = [this] {
        return skipUntil(tok::semi);
    };
    if (consume(tok::kw_PROCEDURE)) {
        return _errorhandler();
    }
    if (expect(tok::identifier)) {
        return _errorhandler();
    }
    ProcedureDeclaration *D = Actions.actOnProcedureDeclaration(Tok.getLocation(), Tok.getIdentifier());
    EnterDeclScope S(Actions, D);
    FormalParamList Params;
    Decl *RetType = nullptr;
    advance();
    if(Tok.is(tok::l_paren)) {
        if (parseFormalParameters(Params, RetType)) {
            return _errorhandler();
        }
    }
    Actions.actOnProcedureHeading(D, Params, RetType);
    if (expect(tok::semi)) {
        return _errorhandler();
    }
    DeclList Decls;
    StmtList Stmts;
    advance();
    if (parseBlock(Decls, Stmts)) {
        return _errorhandler();
    }
    if (expect(tok::identifier)) {
        return _errorhandler();
    }
    Actions.actOnProcedureDeclaration(D, Tok.getLocation(), Tok.getIdentifier(), Decls, Stmts);
    ParentDecls.push_back(D);
    advance();
    return false;
}

bool Parser::parseFormalParameters(FormalParamList &Params, Decl *&RetType) {
    auto _errorhandler = [this] {
        return skipUntil(tok::semi);
    };
    if (consume(tok::l_paren)) {
        return _errorhandler();
    }
    if(Tok.isOneOf(tok::kw_VAR, tok::identifier)) {
        if (parseFormalParameterList(Params)) {
            return _errorhandler();
        }
    }
    if (consume(tok::r_paren)) {
        return _errorhandler();
    }
    if (Tok.is(tok::colon)) {
        advance();
        if (parseQualident(RetType)) {
            return _errorhandler();
        }
    }
    return false;
}

bool Parser::parseFormalParameterList(FormalParamList &Params) {
    auto _errorhandler = [this] {
        return skipUntil(tok::r_paren);
    };
    if (parseFormalParameter(Params)) {
        return _errorhandler();
    }
    while (Tok.is(tok::semi)) {
        advance();
        if (parseFormalParameter(Params)) {
            return _errorhandler();
        }
    }
    return false;
}
