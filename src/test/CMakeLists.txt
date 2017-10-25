add_executable(lru_test lru_test.cpp)
target_link_libraries(lru_test LINK_PUBLIC lru)

set_target_properties( lru_test
                      PROPERTIES
                      COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11"
)

