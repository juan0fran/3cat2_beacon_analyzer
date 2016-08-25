cmake_minimum_required(VERSION 2.6)

##project
project(C_Test C)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_executable (kiss ${SOURCES})
add_executable (isis_raw apps/isis_reader.c src/3cat2_telemetry.c src/util.c)