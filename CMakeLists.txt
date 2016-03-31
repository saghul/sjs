cmake_minimum_required(VERSION 2.8)

project(sjs)

set(CMAKE_C_FLAGS "-O2 -g -pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer")

include_directories(${CMAKE_SOURCE_DIR}/include)

# libsjs
#
add_library(sjs SHARED
    src/duktape.c
    src/modules.c
    src/vm.c
    src/version.c
)

target_include_directories(sjs
    PRIVATE ${CMAKE_SOURCE_DIR}/include/sjs
)

set_target_properties(sjs
    PROPERTIES COMPILE_DEFINITIONS "_GNU_SOURCE"
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(sjs PROPERTIES
        MACOSX_RPATH                ON
        BUILD_WITH_INSTALL_RPATH    1
        INSTALL_NAME_DIR            "@rpath"
    )
endif()


# sjs
#
add_executable(sjs_cli
    src/cli/linenoise.c
    src/cli/main.c
)

set_target_properties(sjs_cli PROPERTIES
    OUTPUT_NAME "sjs"
    COMPILE_DEFINITIONS "_GNU_SOURCE"
)

target_link_libraries(sjs_cli
    sjs
    m
)
if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    target_link_libraries(sjs_cli
        dl
    )
endif()


# install target
#

install(TARGETS sjs sjs_cli
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)

install(DIRECTORY include/sjs
        DESTINATION include)

install(DIRECTORY modules
        DESTINATION lib/sjs
        PATTERN "*.c" EXCLUDE
        PATTERN "*.h" EXCLUDE
        PATTERN "*.swp" EXCLUDE)

# modules
#

# path
add_library(_path MODULE
    modules/_path.c
)
target_link_libraries(_path
    sjs
)
set_target_properties(_path PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)

