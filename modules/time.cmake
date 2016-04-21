
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
        DESTINATION lib/sjs/modules)

