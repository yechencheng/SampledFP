add_definitions(-DPIN_ROOT="${PIN_ROOT}/")

add_library(sampler SHARED sampler.cpp)
add_library(compressor SHARED compressor.cpp)

add_executable(USampleWSS USampleWSS.cpp)
add_executable(RandomSampleWSS RandomSampleWSS.cpp)
add_executable(RandomSampleWSSResult RandomSampleWSSResult.cpp)

target_link_libraries(compressor lz4)
target_link_libraries(sampler compressor)
target_link_libraries(USampleWSS sampler boost_program_options compressor)
target_link_libraries(RandomSampleWSS sampler boost_program_options compressor)
target_link_libraries(RandomSampleWSSResult sampler boost_program_options compressor)

add_subdirectory(test)
add_subdirectory(lru)
add_subdirectory(trace_pin)
add_subdirectory(plot)
