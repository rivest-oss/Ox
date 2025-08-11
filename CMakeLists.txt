cmake_minimum_required(VERSION 3.15)

project(ox-test)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Programme
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} "${sources}")
target_link_libraries(${PROJECT_NAME} raylib)
target_compile_definitions(${PROJECT_NAME} PRIVATE OX_TEST)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
