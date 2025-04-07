#ifndef TINYLANG_LEXER_TOKEN_H
#define TINYLANG_LEXER_TOKEN_H
#include "tinylang/Basic/LLVM.h"
#include "tinylang/Basic/TokenKinds.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/ADT/StringRef.h"


namespace tinylang{
using namespace llvm;

class Lexer;

class Token
{
private:
    friend class Lexer;
    const char *Ptr; // Pointer to start of token
    size_t Length;
    tok::TokenKind Kind;
public:
    tok::TokenKind getKind() const noexcept { return Kind; }
    void setKind(tok::TokenKind K) { Kind = K; }
    size_t getLength() const noexcept { return Length; }

    bool is(tok::TokenKind K) const { return K == Kind; }
    /* Old code: Its outdated. Use C++17 fold expressions instead
    bool isOneOf(tok::TokenKind K1, tok::TokenKind K2) const { return is(K1) || is(K2); }
    template<typename... Ts>
    bool isOneOf(tok::TokenKind K1, tok::TokenKind K2, Ts... Ks) const {
        return is(K1) || isOneOf(K2, Ks...);
    }*/
    template<typename...Tokens>
    bool isOneOf(Tokens&&... Toks){
        return (... || is(Toks));
    }

    const char *getName() const {
        return tok::getTokenName(Kind);
    }

    // The SMLoc instance, which denotes the source position in messages, 
    // is created from the pointer to the token
    SMLoc getLocation() const {
        return SMLoc::getFromPointer(Ptr);
    }
    
    StringRef getIdentifier(){
        assert(is(tok::identifier) && "Cannot get identifier of non-identifier");
        return StringRef(Ptr, Length);
    }

    StringRef getLiteralData(){
        assert(isOneOf(tok::integer_literal, tok::string_literal) && "Cannot get literal data of non-literal");
        return StringRef(Ptr, Length);
    }
};
} //namespace tinylang
#endif