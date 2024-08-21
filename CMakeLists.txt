cmake_minimum_required(VERSION 3.10)
project(SteganoPass VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
  )
  FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) 
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

# source files
add_executable(SteganoPass 
    src/main.cpp
    src/Steganography.cpp
)

target_link_libraries(SteganoPass raylib)

# include directories
target_include_directories(SteganoPass PRIVATE ${CMAKE_SOURCE_DIR}/src)

# If you're on Windows and using MinGW, you might need this
if(MINGW)
    target_link_libraries(SteganoPass winmm)
endif()

# Copy raylib.dll to output directory on Windows
if(WIN32)
    add_custom_command(TARGET SteganoPass POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib>
        $<TARGET_FILE_DIR:SteganoPass>
    )
endif()