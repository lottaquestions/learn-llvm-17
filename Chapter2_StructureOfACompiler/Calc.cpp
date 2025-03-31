#include "CodeGen.h"
#include "Parser.h"
#include "Sema.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

/* LLVM has its own system for parsing command line arguments.
 * Each component can add its own command line options which are
 * registered with a global command line parser.
 * The command line options are declared as static variables.
 */

 static llvm::cl::opt<std::string> Input(llvm::cl::Positional, llvm::cl::desc("<input expression>"), llvm::cl::init(""));

 int main(int argc, const char **argv){
    llvm::InitLLVM X(argc, argv);
    llvm::cl::ParseCommandLineOptions(argc, argv, "calc - the expression compiler \n");

    Lexer Lex(Input);
    // Parsing and Syntactic analysis
    Parser Parser(Lex);
    AST *Tree = Parser.parse();
    if (!Tree || Parser.hasError())
    {
        llvm::errs() << "Syntax errors occured\n";
        return 1;
    }

    // Semantic anlaysis
    Sema Semantic;
    if(Semantic.semantic(Tree)){
        llvm::errs() << "Sematinc errors occurred \n";
        return 1;
    }

    // Code generation
    CodeGen CodeGenerator;
    CodeGenerator.compile(Tree);
    return 0;
 }