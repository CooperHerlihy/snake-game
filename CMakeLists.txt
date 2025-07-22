cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)

project(Snake LANGUAGES CXX)

set(SDL_STATIC ON)
set(SDL_SHARED OFF)
add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")
add_executable(snake ${SRC})
target_link_libraries(snake PRIVATE SDL3::SDL3)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Configuring for GCC or Clang...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "Configuring for MSVC...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT demo)
else()
    message(WARNING "Unknown compiler ID: ${CMAKE_CXX_COMPILER_ID}. Using default flags.")
endif()
