add_executable(calc Calc.cpp CodeGen.cpp Lexer.cpp Parse.cpp Sema.cpp)
target_link_libraries(calc PRIVATE ${llvm_libs})