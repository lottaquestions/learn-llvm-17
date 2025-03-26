#include "Parser.h"

/*
 * The grammar for the language is:
 *   calc : ("with" ident ("," ident)* ":")? expr ;
 *   expr : term (( "+" | "-" ) term)* ;
 *   term : factor (( "*" | "/") factor)* ;
 *   factor : ident | number | "(" expr ")" ;
 *   ident : ([a-zAZ])+ ;
 *   number : ([0-9])+ ;
*/
AST *Parser::parse(){
    AST *Res = parseCalc();
    expect(Token::eoi);
    return Res;
}

// Implements 
//    calc : ("with" ident ("," ident)* ":")? expr ;
AST *Parser::parseCalc(){
    Expr E;
    llvm::SmallVector<llvm::StringRef, 8> Vars;

    // Conditional ("with" ident ("," ident)* ":")? 
    if (Tok.is(Token::KW_with)){
        advance();

        if expect(Token::ident){
            goto _error;
        }
        Vars.push_back(Tok.getText());
        advance();
        while (Tok.is(Token::comma)){
            advance();
            if (expect(Token::ident)){
                goto _error;
            }
            Vars.push_back(Tok.getText());
            advance();
        }
        
        if(consume(Token::colon)){
            goto _error;
        }
    }

    // Expression is non-conditional, i.e. must always be there
    E = parseExpr();
    if (Vars.empty()) {
        return E;
    } else {
        return new WithDecl(Vars, E);
    }

    _error:
    // If we encountered an error when parsing calc (basically an expression), jettison 
    // all tokens until we get to the end then return nothing.
    while (!Tok.is(Token::eoi)){
        advance();
    }
    return nullptr;
    
    
}

// Implements
//   expr : term (( "+" | "-" ) term)* ;
Expr *Parser::parseExpr(){
    Expr *Left = parseTerm();
    while (Tok.isOenOf(Token::plus, Token::minus)){
        BinaryOp::Operator Op = Tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
        advance();
        Expr *Right = parseTerm();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
    
}

// Implements
//   term : factor (( "*" | "/") factor)* ;
Expr *Parser::parseTerm(){
    Expr *Left = parseFactor();
    while (Tok.isOneOf(Token::star, Token::slash)){
        BinaryOperator Op = Token.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
        advance();
        Exp *Right = parseFactor();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
}


// Implements
//   factor : ident | number | "(" expr ")" ;
Expr *Parser::parseFactor(){
    Expr *Res = nullptr;
    switch (Tok.getKind()){
    case Token::number:
        Res = new Factor(Factor::Number, Tok.getText());
        advance();
        break;
    case Token::ident:
        Res = new Factor(Factor::Ident, Tok.getText());
        advance();
        break;
    case Token::l_paren:
        advance();
        Res = parseExpr();
        if(!consume(Token::r_paren)) break;

    default:
        if(!Res) error();
        while(!Token.isOneOf(Token::r_paren, Token::star, Token::plus, Token::minus, Token::slash, Token::eoi))
            advance();

    }
    return Res;
}