#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class ToIRVisitor : public ASTVisitor{
        Module *M; // Each compilation unit is represented in LLVM by the Module class and the visitor has a pointer to this module
        IRBuilder<> Builder; // For easy IR generation
        // To avoid repeated lookups, we cache the needed type instances
        Type *VoidTy;
        Type *Int32Ty;
        PointerType *PtrTy;
        Constant *Int32Zero;

        Value *V; // The current calculated value, which is updated through the tree traversal
        StringMap<Value*> nameMap;// maps a variable name to the value returned from the calc_read() function

        public:
        explicit ToIRVisitor(Module *M) : M(M), Builder(M->getContext()){
            VoidTy = Type::getVoidTy(M->getContext());
            Int32Ty = Type::getInt32Ty(M->getContext());
            PtrTy = PointerType::getUnqual(M->getContext());
            Int32Zero = ConstantInt::get(Int32Ty, 0, true);
        }

        void run(AST *Tree){

            // Defining the function prototype for the "main" function
            FunctionType *MainFty = FunctionType::get(
                Int32Ty, {Int32Ty, PtrTy}, false
            );
            // Defining the function instance for "main"
            Function *MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);

            // Create a basic block, BB, with the label "entry" and attach it to the IR builder
            BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
            Builder.SetInsertPoint(BB);

            // Start the tree traversal
            Tree->accept(*this);

            // Create a function prototype for the calc_write() function
            // calc_write() prints the computed value after tree traversal is completed.
            // It's only parameter is the result of tree traversal which is of Int32Ty
            // LLVM IR: declare void @calc_write(i32)
            FunctionType *CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);

            // Defining the function instance for calc_write()
            Function *CalcWriteFn = Function::Create(CalcWriteFnTy, GlobalValue::ExternalLinkage, "calc_write", M);

            // Call the calc_write() function with the result of the tree traversal
            Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {V});

            // Return 0 from the main() function
            Builder.CreateRet(Int32Zero);
        }

        
        virtual void visit(WithDecl &Node) override {
            // In order to evaluate a WithDecl expression, we first have to obtain the variable
            // from the user. This is done with the calc_read() function.
            // LLVM IR: declare i32 @calc_read(ptr)
            FunctionType *ReadFty = FunctionType::get(Int32Ty, {PtrTy}, false);
            Function *ReadFn = Function::Create(ReadFty, GlobalValue::ExternalLinkage, "calc_read", M);

            // Loop through all the variable names and process them
            for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
            {
                // For each variable, a string with a variable name is created
                StringRef Var = *I;
                Constant *StrText = ConstantDataArray::getString(M->getContext(), Var);
                GlobalVariable *Str = new GlobalVariable(*M, StrText->getType(),
                                                         /*isConstant=*/true,
                                                         GlobalValue::PrivateLinkage, StrText, Twine(Var).concat(".str"));
                //Note: Twine - A lightweight data structure for efficiently representing the
                // concatenation of temporary values as strings.

                // Create an IR call to the calc_read() function
                CallInst *Call = Builder.CreateCall(ReadFty, ReadFn, {Str});

                // Returned value is stored in the nameMap for later use
                nameMap[Var] = Call;
            }

            // After obtaining variables, continue tree traversal with the expression
            Node.getExpr()->accept(*this);
        }

        virtual void visit(Factor &Node) override {
            // A factor is either a variable or a number.
            if(Node.getKind() == Factor::Ident){
                // Values of variables are looked up from nameMap map.
                V = nameMap[Node.getVal()];
            } else {
                // Numbers are converted into integers, which are then 
                // turned into constant integers
                int intval{0};
                Node.getVal().getAsInteger(10, intval);
                V = ConstantInt::get(Int32Ty, intval,true);
            }
        }

        virtual void visit(BinaryOp &Node) override {
            Node.getLeft()->accept(*this);
            Value *Left = V;
            Node.getRight()->accept(*this);
            Value *Right = V;
            // Select the right operator for binary ops
            switch (Node.getOperator()){
            case BinaryOp::Plus:
                V = Builder.CreateNSWAdd(Left, Right);
                break;
            case BinaryOp::Minus:
                V = Builder.CreateNSWSub(Left, Right);
                break;
            case BinaryOp::Mul:
                V = Builder.CreateNSWMul(Left, Right);
                break;
            case BinaryOp::Div:
                V = Builder.CreateSDiv(Left, Right);
                break;
            }
        }
    };
} // unnamed namespace

void CodeGen::compile(AST *Tree){
    LLVMContext Ctx; // Create global context
    Module *M = new Module("calc.expr", Ctx); // Create module
    ToIRVisitor ToIR(M);
    ToIR.run(Tree); // Perform tree traversal
    M->print(outs(), nullptr); // Dump generated IR to console
}