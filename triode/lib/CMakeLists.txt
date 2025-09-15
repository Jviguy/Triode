add_library(triode STATIC
        emulator.cpp cpu.cpp memory.cpp
)

target_include_directories(triode PUBLIC include)

target_link_libraries(triode PUBLIC arch)
