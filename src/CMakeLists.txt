
#set(LinkList General Tapas_Shared ConfigureHelper)
add_definitions(-DCMAKE_TRACER_PERF_DIR="${CMAKE_BINARY_DIR}/other/Tracer/Perf/")
add_definitions(-DCMAKE_TRACER_PIN_DIR="${CMAKE_BINARY_DIR}/other/Tracer/Pin/")
add_definitions(-DPIN_ROOT="${PIN_ROOT}/")
add_definitions(-DCMAKE_PERF_DIR="${CMAKE_PERF_DIR}/")

add_subdirectory(test)
add_subdirectory(lru)
add_subdirectory(sampler_pin)
