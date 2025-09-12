add_library(libarch STATIC
        tryte.cpp
        word.cpp
        include/triode/instructions.h
)
target_include_directories(libarch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)