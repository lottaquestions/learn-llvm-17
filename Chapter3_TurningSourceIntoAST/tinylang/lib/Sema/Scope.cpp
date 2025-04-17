#include "tinylang/Sema/Scope.h"
#include "tinylang/AST/AST.h"

using namespace tinylang;

bool Scope::insert(Decl *Declaration) {
      /// insert - Inserts the specified key/value pair into the map if the key
      /// isn't already in the map. The bool component of the returned pair is true
     /// if and only if the insertion takes place, and the iterator component of
    /// the pair points to the element with key equivalent to the key of the pair
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