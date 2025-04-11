set(LLVM_LINK_COMPONENTS support)

add_tinylang_library(tinylanglexer
Lexer.cpp

LINK_LIBS
tinylangBasic
)