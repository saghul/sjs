cmake_minimum_required(VERSION 2.8)

project(sjs)

set(CMAKE_C_FLAGS "-O2 -g -pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer")

# libsjs
#
add_library(sjs SHARED
    src/duktape.c
    src/modules.c
    src/vm.c
    src/version.c
)

target_include_directories(sjs
    PUBLIC ${CMAKE_SOURCE_DIR}/include/sjs
)

set_target_properties(sjs
    PROPERTIES COMPILE_DEFINITIONS "_XOPEN_SOURCE=500"
)


# sjs
#
add_executable(sjs_cli
    src/cli/linenoise.c
    src/cli/main.c
)

target_include_directories(sjs_cli
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(sjs_cli
    PROPERTIES COMPILE_DEFINITIONS "_GNU_SOURCE"
)

set_target_properties(sjs_cli
    PROPERTIES OUTPUT_NAME "sjs"
)

target_link_libraries(sjs_cli
    sjs
    m
)
