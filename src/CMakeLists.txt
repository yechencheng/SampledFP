add_definitions(-DPIN_ROOT="${PIN_ROOT}/")

add_library(sampler SHARED sampler.cpp)
add_executable(USampleWSS USampleWSS.cpp)

target_link_libraries(USampleWSS LINK_PUBLIC sampler boost_program_options)

add_subdirectory(test)
add_subdirectory(lru)
add_subdirectory(trace_pin)
