
add_library(_io MODULE
    modules/_io.c
)
target_link_libraries(_io
    sjs
)
set_target_properties(_io PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
install(TARGETS _io
        DESTINATION lib/sjs/modules)

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
        DESTINATION lib/sjs/modules)
