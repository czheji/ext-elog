cmake_minimum_required(VERSION 3.6)
cmake_policy(SET CMP0042 NEW)
project(ELOG VERSION 1.0.0)

set (SRC_ELOG
        src/elog.c
        #src/elog_async.c
        src/elog_buf.c
        src/elog_utils.c
        port/elog_port.c
        )
add_library(elog STATIC ${SRC_ELOG})
target_include_directories(elog PUBLIC inc)


install(TARGETS elog EXPORT elog
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION inc
        )
export(EXPORT elog
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ext/elog.cmake"
        )


# tests
#add_executable(elog-test-linux demo/os/linux/main.c )
#target_link_libraries(elog-test-linux elog)