#ifndef TINYLANG_PARSER_PARSER_H
#define TINYLANG_PARSER_PARSER_H

#include "tinylang/Basic/Diagnostic.h"
#include "tinylang/Lexer/Lexer.h"
#include "tinylang/Sema/Sema.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

namespace tinylang {

    class Parser
    {
    private:
        Lexer &Lex;

        Sema &Action;

        Token Tok;

        DiagnosticsEngine &getDiagnostics() const {
            return Lex.getDiagnostics();
        }

        void advance() { Lex.next(Tok); }

        bool expect(tok::TokenKind ExpectedTok) {
            if(Tok.is(ExpectedTok)) {
                return false;
            }

            const char *Expected = tok::getPunctuatorSpelling(ExpectedTok);
            if(!Expected){
                // If not a punctuation check if it is a keyword
                Expected = tok::getKeywordSpelling(ExpectedTok);
            }
            llvm::StringRef Actual(Tok.getLocation().getPointer(), Tok.getLength());
            getDiagnostics().report(Tok.getLocation(), diag::err_expected, Expected, Actual);
            return true;
        }

        bool consume(tok::TokenKind ExpectedTok) {
            if(Tok.is(ExpectedTok)){
                advance();
                return false;
            }
            return true;
        }

        template<typename... Tokens>
        bool skipUntil(Tokens&&... Toks){
            while (true) {
                if (... || Tok.is(Toks)) {
                    return false;
                }

                if (Tok.is(tok::eof)){
                    return true;
                }
                advance();
            }
            return true; // unreachable
        }
    public:
        Parser(Lexer &Lex, Sema &Action);
        
        ModuleDeclaration *parse();
        // TODO: Incomplete. Finish up the rest of the definitions
    };
    
    
} // namespace tinylang


#endif