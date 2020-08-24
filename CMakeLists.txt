cmake_minimum_required(VERSION 3.7)

project(SDL2Test)

cmake_policy(SET CMP0076 NEW)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(SDL2Test ./src/main.cpp)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/include ${SDL2_image_INCLUDE_DIR} ${SDL2_gfx_INCLUDE_DIR})

target_link_libraries(SDL2Test SDL2::SDL2 ${SDL2_image_LIBRARIES} ${SDL2_gfx_LIBRARIES})