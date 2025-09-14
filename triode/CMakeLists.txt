add_library(triode STATIC
        emulator.cpp cpu.cpp memory.cpp
)

target_include_directories(triode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(triode PUBLIC arch)
