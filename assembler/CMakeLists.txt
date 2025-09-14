add_executable(asm main.cpp
        assembler.cpp
        assembler.h
        lexer.cpp
        lexer.h
        parser.cpp
        parser.h
        token.cpp
        token.h)
target_link_libraries(asm PRIVATE arch)
