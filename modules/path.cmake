
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
        DESTINATION lib/sjs/modules)

