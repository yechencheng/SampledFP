add_definitions(-DPIN_ROOT="${PIN_ROOT}/")

add_library(sampler SHARED sampler.cpp)
add_library(compressor SHARED compressor.cpp)

add_executable(USampleWSS USampleWSS.cpp)
add_executable(RandomSampleWSS RandomSampleWSS.cpp)
add_executable(RandomSampleWSSResult RandomSampleWSSResult.cpp)
add_executable(Footprint Footprint.cpp)
add_executable(GenerateTrace GenerateTrace.cpp)
add_executable(SampledFootprint SampledFootprint.cpp)

#INSTALL(TARGETS USampleWSS RandomSampleWSS RandomSampleWSSResult Footprint sampler compressor
#        RUNTIME DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#        LIBRARY DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
#        ARCHIVE DESTINATION ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
#        )

target_link_libraries(compressor lz4)
target_link_libraries(sampler compressor)
target_link_libraries(USampleWSS sampler boost_program_options compressor)
target_link_libraries(RandomSampleWSS sampler boost_program_options compressor)
target_link_libraries(RandomSampleWSSResult sampler boost_program_options compressor)
target_link_libraries(Footprint sampler boost_program_options compressor)
target_link_libraries(GenerateTrace sampler boost_program_options compressor)
target_link_libraries(SampledFootprint boost_program_options compressor)

add_subdirectory(test)
add_subdirectory(lru)
add_subdirectory(trace_pin)
add_subdirectory(plot)
add_subdirectory(analyze)
