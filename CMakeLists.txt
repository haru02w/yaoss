cmake_minimum_required(VERSION 3.5)

project(main LANGUAGES C)
# project(template LANGUAGES CXX)

# Set the C/C++ standard required
set(CMAKE_C_STANDARD 17)
#set(CMAKE_CXX_STANDARD 11)

# set clang-format flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set cmake debug target
set(CMAKE_BUILD_TYPE Debug)

# set flags to compiler
# set(CMAKE_CXX_FLAGS ...)
set(CMAKE_C_FLAGS
  "${CMAKE_C_FLAGS} -Wall -Wextra -Wuninitialized -g")

# Ensure the build and source directory exists
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY "src")
# grabs all source files
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.c")
# file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

find_package(Curses REQUIRED)
find_library(MENU_LIBRARY menu)
include_directories(${CURSES_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${MENU_LIBRARY} ${CURSES_LIBRARIES} m)

option(TESTING "Enable tests." OFF)

if(TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
