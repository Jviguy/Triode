add_executable(triode_tests
        cpu.cpp
)

target_link_libraries(triode_tests PRIVATE Catch2::Catch2WithMain triode)