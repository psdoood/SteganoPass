cmake_minimum_required(VERSION 3.14)
project(SteganoPass VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest QUIET)

if(NOT GTest_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()

add_library(tinyaes STATIC
    ${CMAKE_SOURCE_DIR}/external/tinyaes/aes.c
)
target_include_directories(tinyaes PUBLIC
    ${CMAKE_SOURCE_DIR}/external/tinyaes
)
target_compile_definitions(tinyaes PUBLIC CBC=1)

#Main executable
add_executable(SteganoPass
    src/main.cpp
    src/Steganography.cpp
    src/stb_impl.cpp
    src/Crypto.cpp
)

target_include_directories(SteganoPass PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tinyaes
)

target_link_libraries(SteganoPass PRIVATE tinyaes)

if(MINGW)
    target_link_libraries(SteganoPass winmm)
endif()

enable_testing()

#Test executable
add_executable(
  SteganoPass_test
  tests/crypto_test.cpp
  tests/steganography_test.cpp
  src/Crypto.cpp
  src/Steganography.cpp
  src/stb_impl.cpp
)

if(GTest_FOUND)
  target_link_libraries(SteganoPass_test GTest::gtest_main)
else()
  target_link_libraries(SteganoPass_test gtest_main)
endif()

target_link_libraries(SteganoPass_test tinyaes)

target_include_directories(SteganoPass_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/external/stb
    ${CMAKE_SOURCE_DIR}/external/tinyaes
)

include(GoogleTest)
gtest_discover_tests(SteganoPass_test)