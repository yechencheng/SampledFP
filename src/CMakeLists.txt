add_definitions(-DPIN_ROOT="${PIN_ROOT}/")

add_library(sampler SHARED sampler.cpp)
add_executable(SampledWSS SampledWSS.cpp)

target_link_libraries(SampledWSS LINK_PUBLIC sampler)

add_subdirectory(test)
add_subdirectory(lru)
add_subdirectory(sampler_pin)
