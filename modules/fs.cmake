
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
install(TARGETS _fs
        DESTINATION lib/sjs/modules)

