#include "tinylang/Basic/TokenKinds.h"
#include "llvm/Support/ErrorHandling.h"

using namespace tinylang;
static const char * const  TokNames[] = {
    #define TOK(ID) #ID,
    #define KEYWORD(ID, FLAG) #ID, /* The # before ID turns the ID into a string literal*/
    #include "tinylang/Basic/TokenKinds.def"
    nullptr /* accounts for the NUM_TOKEN enumeration member*/
};

const char *tok::getTokenName(TokenKind Kind){
    return TokNames[Kind];
}

const char *tok::getPunctuatorSpelling(TokenKind Kind){
    switch (Kind){
        #define PUNCTUATOR(ID, SP) case ID: return SP;
        #include "tinylang/Basic/TokenKinds.def"
        default: break;
    }
    return nullptr;
}

const char *tok::getKeywordSpelling(TokenKind Kind){
    switch (Kind){
        #define KEYWORD(ID, FLAG) case kw_ ## ID: return #ID;
        #include "tinylang/Basic/TokenKinds.def"
        default: break;
    }
    return nullptr;
}