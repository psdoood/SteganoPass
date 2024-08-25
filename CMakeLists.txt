cmake_minimum_required(VERSION 3.10)
project(SteganoPass VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(tinyaes STATIC
    ${CMAKE_SOURCE_DIR}/external/tinyaes/aes.c
)
target_include_directories(tinyaes PUBLIC
    ${CMAKE_SOURCE_DIR}/external/tinyaes
)

target_compile_definitions(tinyaes PUBLIC CBC=1)

# source files
add_executable(SteganoPass
    src/main.cpp
    src/Steganography.cpp
    src/stb_impl.cpp
    src/Crypto.cpp 
)

# include directories
target_include_directories(SteganoPass PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tinyaes 
)

# link libraries
target_link_libraries(SteganoPass PRIVATE tinyaes)

if(MINGW)
    target_link_libraries(SteganoPass winmm)
endif()