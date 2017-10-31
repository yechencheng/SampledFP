add_executable(lru_test lru_test.cpp)
add_executable(USampleWSS_test USampleWSS_test.cpp)
add_executable(compressor_test compressor_test.cpp)

target_link_libraries(lru_test lru)
target_link_libraries(compressor_test compressor)
target_link_libraries(USampleWSS_test compressor)
set_target_properties( lru_test
                        compressor_test
                      PROPERTIES
                      COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11"
)

set_target_properties(USampleWSS_test
                    PROPERTIES
                    COMPILE_FLAGS "-O0 -g -std=c++11")