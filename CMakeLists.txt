cmake_minimum_required(VERSION 3.7)

project(SDL2Test)

cmake_policy(SET CMP0076 NEW)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include ${SDL2_INCLUDE_DIR} ${SDL2_image_INCLUDE_DIR} ${SDL2_gfx_INCLUDE_DIR})

file(GLOB SOURCES src/*.cpp)

if (${EMSCRIPTEN})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/public)
  set(CMAKE_EXECUTABLE_SUFFIX .html)

  add_executable(${PROJECT_NAME} ${SOURCES})  
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
  add_executable(${PROJECT_NAME} ${SOURCES})
  target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_image_LIBRARIES} ${SDL2_gfx_LIBRARIES})  
endif()