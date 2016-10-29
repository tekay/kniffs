cmake_minimum_required(VERSION 3.5)
project(kniffs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    src/ball.cpp
    src/ball.h
    src/field.cpp
    src/field.h
    src/lbutton.cpp
    src/lbutton.h
    src/loader.cpp
    src/loader.h
    src/ltexture.cpp
    src/ltexture.h
    src/main.cpp)

# Find and include external libraries
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIR}
                    ${SDL2_IMAGE_INCLUDE_DIR}
                    ${SDL2_TTF_INCLUDE_DIR})

add_executable(kniffs ${SOURCE_FILES})

# Link libraries
target_link_libraries(kniffs ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES})
