
# errno
#

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
        DESTINATION lib/sjs/modules
)


# fs
#

add_library(_fs MODULE
    modules/_fs.c
)
target_link_libraries(_fs
    sjs
)
set_target_properties(_fs PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(_fs PROPERTIES
        COMPILE_DEFINITIONS "_POSIX_C_SOURCE=200112"
    )
endif()
install(TARGETS _fs
        DESTINATION lib/sjs/modules
)


# io
#

add_library(_select MODULE
    modules/_select.c
)
target_link_libraries(_select
    sjs
)
set_target_properties(_select PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
install(TARGETS _select
        DESTINATION lib/sjs/modules
)
add_library(_poll MODULE
    modules/_poll.c
)
target_link_libraries(_poll
    sjs
)
set_target_properties(_poll PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
    COMPILE_DEFINITIONS "_GNU_SOURCE"
)
install(TARGETS _poll
        DESTINATION lib/sjs/modules
)


# net
#

add_library(_socket MODULE
    modules/_socket.c
)
target_link_libraries(_socket
    sjs
)
set_target_properties(_socket PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
install(TARGETS _socket
        DESTINATION lib/sjs/modules)

add_library(_gai MODULE
    modules/_gai.c
)
target_link_libraries(_gai
    sjs
)
set_target_properties(_gai PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(_gai PROPERTIES
        COMPILE_DEFINITIONS "_POSIX_C_SOURCE=200112"
    )
endif()
install(TARGETS _gai
        DESTINATION lib/sjs/modules
)


# os
#

add_library(_os MODULE
    modules/_os.c
)
target_link_libraries(_os
    sjs
)
set_target_properties(_os PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set_target_properties(_os PROPERTIES
        COMPILE_DEFINITIONS "_GNU_SOURCE"
    )
endif()
install(TARGETS _os
        DESTINATION lib/sjs/modules
)


# path
#

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
install(TARGETS _path
        DESTINATION lib/sjs/modules
)


# time
#

add_library(_time MODULE
    modules/_time.c
)
target_link_libraries(_time
    sjs
)
set_target_properties(_time PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
install(TARGETS _time
        DESTINATION lib/sjs/modules
)
