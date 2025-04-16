#ifndef TINYLANG_SEMA_SCOPE_H
#define TINYLANG_SEMA_SCOPE_H
#include "tinylang/Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace tinylang {
    class Decl;

    class Scope {
    private:
        Scope *Parent;
        StringMap<Decl *> Symbols;

    public:
        Scope(Scope *Parent = nullptr) : Parent(Parent) {}
        bool insert(Decl *Declaration);
        Decl *lookup(StringRef Name);

        Scope *getParent() noexcept { return Parent; }
    };
    
} // namespace tinylang
#endif