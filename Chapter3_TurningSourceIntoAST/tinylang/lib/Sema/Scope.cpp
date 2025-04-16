#include "tinylang/Sema/Scope.h"
#include "tinylang/AST/AST.h"

using namespace tinylang;

bool Scope::insert(Decl *Declaration) {
    return Symbols.insert(std::pair<StringRef, Decl *>(Declaration->getName(), Declaration)).second;
}

Decl *Scope::lookup(StringRef Name) {
    Scope *CurScope = this;

    while (CurScope) {
        StringMap<Decl *>::const_iterator It = CurScope->Symbols.find(Name);
        if(It != CurScope->Symbols.end()){
            return It->second;
        }
        CurScope = CurScope->getParent();
    }
    return nullptr;
}