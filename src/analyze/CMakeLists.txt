add_executable(WholeTraceFeq WholeTraceFeq.cpp)
target_link_libraries(WholeTraceFeq LINK_PUBLIC boost_program_options compressor sampler)

add_executable(ShowFP ShowFP.cpp)
target_link_libraries(ShowFP LINK_PUBLIC )

#add_executable(getFeq getFeq.cpp)
#target_link_libraries(getFeq LINK_PUBLIC boost_program_options compressor sampler)

