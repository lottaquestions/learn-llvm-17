## Compilation instructions
1. Go outside of the `src` directory and create a build directory in which the compilation will be done:

    ```
    cd ~/learn-llvm-17/Chapter2_StructureOfACompiler/
    mkdir build
    cd build
    ```

2. Build the `calc` app from within the `build` directory

    ```
    cmake -GNinja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLLVM_DIR=/home/nick/Software/llvm/installation/lib/cmake/llvm ../
    ninja
    ```

   The binary will be generated in `build/src/` as can be seen from the below `find` output:

    ```
    $ file `find . -name "calc"`
    ./src/calc: ELF 64-bit LSB pie executable, x86-64, version 1 (GNU/Linux), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 3.2.0, not stripped
    ```

## Compiling an input program
1. Compile an input program such as the example shown below. The generated IR is fed directly into `llc` which is LLVMs static backend compiler. `llc` produces an object file as its output
    ```
    ./calc "with a: a*3" | llc -filetype=obj -relocation-model=pic -o expr.o
    ```

2. The generated object file can then be linked against the runtime library using a C compiler

    ```
    clang -o expr expr.o ../../src/RTCalc.c
    ```

## Running a compiled and linked input program:

```
$ ./expr 
Enter a value for a: 5
The result is: 15
```