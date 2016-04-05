
add_library(time MODULE
    modules/time.c
)
target_link_libraries(time
    sjs
)
set_target_properties(time PROPERTIES
    PREFIX ""
    SUFFIX ".jsdll"
)
install(TARGETS time
        DESTINATION lib/sjs/modules)

