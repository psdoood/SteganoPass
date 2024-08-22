cmake_minimum_required(VERSION 3.10)
project(SteganoPass VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# source files
add_executable(SteganoPass
    src/main.cpp
    src/Steganography.cpp
    src/stb_impl.cpp 
)

# include directories
target_include_directories(SteganoPass PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/stb
)

if(MINGW)
    target_link_libraries(SteganoPass winmm)
endif()