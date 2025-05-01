#include "tinylang/Basic/Diagnostic.h"

using namespace tinylang;

namespace {
    const char *DiagnosticText[] = {
        #define DIAG(ID, Level, Msg) Msg, /* only return the Msg part*/
        #include "tinylang/Basic/Diagnostic.def"
    };
    SourceMgr::DiagKind DiagnosticKind[] = {
        #define DIAG(ID, Level, Msg) SourceMgr::DK_##Level, /* return the level enum from LLVM*/
        #include "tinylang/Basic/Diagnostic.def"
    };
} // namespace 

const char *DiagnosticsEngine::getDiagnosticText(unsigned DiagID){
    return DiagnosticText[DiagID];
}

SourceMgr::DiagKind DiagnosticsEngine::getDiagnosticKind(unsigned DiagID){
    return DiagnosticKind[DiagID];
}