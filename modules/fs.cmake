
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
        DESTINATION lib/sjs/modules)

