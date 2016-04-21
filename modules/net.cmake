
# socket
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

# gai
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
        DESTINATION lib/sjs/modules)

