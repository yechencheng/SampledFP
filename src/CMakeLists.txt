add_definitions(-DPIN_ROOT="${PIN_ROOT}/")

add_library(sampler SHARED sampler.cpp)
add_library(compressor SHARED compressor.cpp)

add_executable(USampleWSS USampleWSS.cpp)
add_executable(RandomSampleWSS RandomSampleWSS.cpp)

target_link_libraries(compressor LINK_PUBLIC lz4)
target_link_libraries(USampleWSS LINK_PUBLIC sampler boost_program_options)
target_link_libraries(RandomSampleWSS LINK_PUBLIC sampler boost_program_options)

add_subdirectory(test)
add_subdirectory(lru)
add_subdirectory(trace_pin)
add_subdirectory(plot)