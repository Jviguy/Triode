add_executable(libarch_tests tryte.cpp word.cpp
        word.cpp
        trits.cpp)
target_link_libraries(libarch_tests PRIVATE Catch2::Catch2WithMain libarch)
