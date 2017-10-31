add_executable(lru_test lru_test.cpp)
add_executable(USampleWSS_test USampleWSS_test.cpp)
add_executable(compressor_test compressor_test.cpp)

target_link_libraries(lru_test LINK_PUBLIC lru)
target_link_libraries(compressor_test compressor)

set_target_properties( lru_test
                        compressor_test
                      PROPERTIES
                      COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11"
)

set_target_properties(USampleWSS_test
                    PROPERTIES
                    COMPILE_FLAGS "-O0 -g")