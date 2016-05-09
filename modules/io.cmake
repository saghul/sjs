
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
        DESTINATION lib/sjs/modules)
