
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
install(TARGETS _errno
        DESTINATION lib/sjs/modules)

