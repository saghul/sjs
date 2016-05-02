cmake_minimum_required(VERSION 2.8.12)

project(sjs)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_C_FLAGS "-pedantic -std=c99 -Wall -fstrict-aliasing -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -g")

include_directories(${CMAKE_SOURCE_DIR}/include)

# libsjs
#
set(SOURCES
    src/duktape.c
    src/modules.c
    src/util.c
    src/vm.c
    src/version.c
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(SOURCES ${SOURCES}
        src/platform/darwin.c
    )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(SOURCES ${SOURCES}
        src/platform/linux.c
    )
endif()

add_library(sjs SHARED ${SOURCES})

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
include(modules/path.cmake)

# errno
include(modules/errno.cmake)

# net
include(modules/net.cmake)

# time
include(modules/time.cmake)

# io
include(modules/io.cmake)

