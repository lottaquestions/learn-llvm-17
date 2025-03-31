#ifndef PARSER_H
#define PARSER_H
#include "AST.h"
#include "Lexer.h"
// The coding guidelines from LLVM forbid the use of the <iostream> library, 
// therefore, the header of the equivalent LLVM functionality is included
// e.g. llvm::errs() is defined in this file
#include "llvm/Support/raw_ostream.h"

class Parser{
    Lexer &Lex;
    Token Tok;
    bool HasError;

    void error(){
        llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
        HasError = true;
    }

    void advance() { Lex.next(Tok); }

    bool expect(Token::TokenKind Kind){
        if(Tok.getKind() != Kind){
            error();
            return true;
        }
        return false;
    }

    bool consume(Token::TokenKind Kind){
        if (expect(Kind)){
            return true;
        }
        advance();
        return false;
        
    }

    AST *parseCalc();
    Expr *parseExpr();
    Expr *parseTerm();
    Expr *parseFactor();

    public:
    Parser(Lexer &Lex) : Lex(Lex), HasError(false){
        advance();
    }

    bool hasError() { return HasError; }

    AST *parse();
};
#endif