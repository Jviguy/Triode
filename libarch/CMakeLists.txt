add_library(libarch STATIC
        types.cpp)
target_include_directories(libarch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)