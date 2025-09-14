add_executable(arch_tests tryte.cpp word.cpp
        word.cpp
        trits.cpp)
target_link_libraries(arch_tests PRIVATE Catch2::Catch2WithMain arch)
