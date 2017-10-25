cmake_minimum_required(VERSION 2.8)

project("Tapas")

message(${CMAKE_SYSTEM_PROCESSOR})
message(${CMAKE_CXX_COMPILER_ID})
set(TAPAS_MAJOR_VERSION 0)
set(TAPAS_MINOR_VERSION 1)
set(TAPAS_PATCH_VERSION 0)
set(TAPAS_VERSION ${TAPAS_MAJOR_VERSION}.${TAPAS_MINOR_VERSION}.${TAPAS_PATCH_VERSION})

set(CMAKE_C_FLAGS "-g -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} --std=c++11")

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
