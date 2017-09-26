set(SRCS
    engine/EntityCheck.cpp
    main.cpp
)

set(HEADERS
    
)

create_directory_groups(${SRCS} ${HEADERS})

add_executable(Tests ${SRCS} ${HEADERS})
target_link_libraries(Tests Engine catch)
set_property(TARGET Tests PROPERTY CXX_STANDARD 11)
set_property(TARGET Tests PROPERTY CXX_STANDARD_REQUIRED ON)
