add_library(arch STATIC
        tryte.cpp
        word.cpp
        trit.cpp
        ../tasm/app/main.cpp
)
target_include_directories(arch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)