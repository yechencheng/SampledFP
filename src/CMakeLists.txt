#set(LinkList General Tapas_Shared ConfigureHelper)
add_definitions(-DCMAKE_TRACER_PERF_DIR="${CMAKE_BINARY_DIR}/other/Tracer/Perf/")
add_definitions(-DCMAKE_TRACER_PIN_DIR="${CMAKE_BINARY_DIR}/other/Tracer/Pin/")
add_definitions(-DPIN_ROOT="${PIN_ROOT}/")
add_definitions(-DCMAKE_PERF_DIR="${CMAKE_PERF_DIR}/")

set_target_properties(
                      GenTrace
                      PROPERTIES
                      COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11"
)


target_link_libraries(TraceProfilePOWER8 LINK_PUBLIC ${CommonLib} TapasPOWER8)

set_target_properties(
                        GenTrace
                        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11"
                        )

