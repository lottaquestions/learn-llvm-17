#ifndef TINYLANG_LEXER_LEXER_H
#define TINYLANG_LEXER_LEXER_H

#include "tinylang/Basic/Diagnostic.h"
#include "tinylang/Basic/LLVM.h"
#include "tinylang/Lexer/Token.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"

namespace tinylang{
    class KeywordFilter{
        llvm::StringMap<tok::TokenKind> HashTable;

        void addKeyword(StringRef Keyword, tok::TokenKind TokenCode);

        public:
        void addKeywords();

        tok::TokenKind getKeyword(StringRef Name, tok::TokenKind DefaultTokenCode == tok::unknown){
            if (auto Result = HashTable.find(Name) ; Result != HashTable.end()){
                return Result->second;
            }
            return DefaultTokenCode;
        }
    };

    class Lexer
    {
    private:
        SourceMgr &SrcMgr;
        DiagnosticsEngine &Diags;

        const char *CurPtr;
        StringRef CurBuf;

        /// CurBuffer - This is the current buffer index we are
        /// lexing from as managed by the SrcMgr object
        unsigned CurBuffer = 0;

        KeywordFilter Keywords;
    public:
        Lexer(SourceMgr &SrcMgr, DiagnosticsEngine &Diags) : SrcMgr(SrcMgr), Diags(Diags){
            CurBuffer = SrcMgr.getMainFileID();
            CurBuf = SrcMgr.getMemoryBuffer(CurBuffer)->getBuffer();
            CurPtr = CurBuf.begin();
            Keywords.addKeywords();
        }

        DiagnosticsEngine &getDiagnostics() const noexcept {
            return Diags;
        }

        /// Returns the next token from the input
        void next(Token &Result);

        /// Get source code buffer
        StringRef getBuffer() const { return CurBuf; }

        private:
        void identifier(Token &Result);
        void number (Token &Result);
        void string (Toekn &Result);
        void comment();

        SMLoc getLoc() { return SMLoc::getFromPointer(CurPtr); }

        void formToken(Token &Result, const char *TokEnd, tok::TokenKind Kind);
        
    };
    
} // namespace tinylang

#endif