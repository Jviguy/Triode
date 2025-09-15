add_library(asm STATIC
        assembler.cpp
        lexer.cpp
        parser.cpp
        token.cpp
)
target_include_directories(asm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(asm PRIVATE arch)
