add_library(libarch STATIC
        tryte.cpp)
target_include_directories(libarch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)