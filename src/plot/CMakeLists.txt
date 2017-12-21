add_executable(histWSS histWSS.cpp)
add_executable(getFP getFP.cpp)
add_executable(ShowFPCurve ShowFPCurve.cpp)

target_link_libraries(histWSS LINK_PUBLIC boost_program_options compressor)
target_link_libraries(getFP LINK_PUBLIC boost_program_options compressor)
