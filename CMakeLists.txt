cmake_minimum_required(VERSION 2.8.12)

project(sjs)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Building in ${CMAKE_BUILD_TYPE} mode")
message(STATUS "Building with ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} on ${CMAKE_SYSTEM}")

set(CMAKE_C_FLAGS "$ENV{CFLAGS} -pedantic -std=c99 -Wall -Wextra -fstrict-aliasing -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Werror -O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2 -g")

include_directories(${CMAKE_SOURCE_DIR}/include)


# Version
#
set(SJS__VERSION_MAJOR 0)
set(SJS__VERSION_MINOR 2)
set(SJS__VERSION_PATCH 0)
set(SJS__VERSION_SUFFIX "")
configure_file (
    "${CMAKE_SOURCE_DIR}/include/sjs/version.h.in"
    "${CMAKE_SOURCE_DIR}/include/sjs/version.h"
)


# Defines for system.build
#
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(SJS__BUILD_COMPILER "GCC")
else()
    set(SJS__BUILD_COMPILER "${CMAKE_C_COMPILER_ID}")
endif()
set(SJS__BUILD_COMPILER_VERSION "${CMAKE_C_COMPILER_VERSION}")
set(SJS__BUILD_TYPE "${CMAKE_BUILD_TYPE}")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(SJS__BUILD_CFLAGS "${CMAKE_C_FLAGS_DEBUG}")
else()
    set(SJS__BUILD_CFLAGS "${CMAKE_C_FLAGS_RELEASE}")
endif()
set(SJS__BUILD_SYSTEM "${CMAKE_SYSTEM}")
string(TIMESTAMP SJS__BUILD_TIMESTAMP UTC)
configure_file (
    "${CMAKE_SOURCE_DIR}/include/sjs/build.h.in"
    "${CMAKE_SOURCE_DIR}/include/sjs/build.h"
)


# RPATH shenanigans
#
set(CMAKE_MACOSX_RPATH OFF)
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# append /usr/local/lib to the implicit link locations
list(APPEND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
    /usr/local/lib
)
# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()


add_definitions(
    -D_GNU_SOURCE
    -D_LARGEFILE_SOURCE
    -D_FILE_OFFSET_BITS=64
)


# duktape
#
add_library(duktape STATIC
    src/duktape.c
)

target_include_directories(duktape
    PRIVATE ${CMAKE_SOURCE_DIR}/include/sjs
)

set_target_properties(duktape
    PROPERTIES COMPILE_FLAGS "-fPIC -Wno-unused-function"
)


# libsjs
#
set(SOURCES
    src/modules.c
    src/system.c
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

target_link_libraries(sjs
    duktape
    m
)
if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    target_link_libraries(sjs
        dl
        rt
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
)

target_link_libraries(sjs_cli
    sjs
)

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

include(modules/modules.cmake)
