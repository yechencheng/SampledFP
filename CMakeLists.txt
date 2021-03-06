cmake_minimum_required(VERSION 2.8)

project("Tapas")

set(TAPAS_MAJOR_VERSION 0)
set(TAPAS_MINOR_VERSION 1)
set(TAPAS_PATCH_VERSION 0)
set(TAPAS_VERSION ${TAPAS_MAJOR_VERSION}.${TAPAS_MINOR_VERSION}.${TAPAS_PATCH_VERSION})

if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif(APPLE)

find_package(Boost 1.64.0 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

find_library(LZ4_LIB lz4)
if(NOT LZ4_LIB)
    message(FATAL_ERROR "lz4 library not found")
endif()

set(CMAKE_C_FLAGS "-g -O3 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} --std=c++11")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# PinTracer
# Note: only support for linux
if(${CMAKE_SYSTEM} MATCHES "Linux")
    set(BUILD_PIN_TRACER OFF CACHE STRING "Build Pin based tracer?" FORCE)
    set(PIN_ROOT "$ENV{PIN_ROOT}")
endif()

# include path
include_directories(${CMAKE_SOURCE_DIR}/include)

# source files
add_subdirectory(src)
