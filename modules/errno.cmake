
add_library(_errno MODULE
    modules/_errno.c
)
target_link_libraries(_errno
    sjs
)
set_target_properties(_errno PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(_errno PROPERTIES
        COMPILE_DEFINITIONS "_POSIX_C_SOURCE=200112"
    )
endif()
install(TARGETS _errno
        DESTINATION lib/sjs/modules)

