include(ExternalProject)
ExternalProject_Add(Sampler
                    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
                    DOWNLOAD_COMMAND ""
                    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                    BUILD_COMMAND make -C ${CMAKE_CURRENT_BINARY_DIR}
                    INSTALL_COMMAND ""
                    )
