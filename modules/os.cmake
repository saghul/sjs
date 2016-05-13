
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
install(TARGETS _os
        DESTINATION lib/sjs/modules)

