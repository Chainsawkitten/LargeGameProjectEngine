set(SRCS
    Simulator.cpp
)

set(HEADERS
    Simulator.hpp
)

create_directory_groups(${SRCS} ${HEADERS})

add_library(Physics STATIC ${SRCS} ${HEADERS})
target_link_libraries(Physics bullet)
set_property(TARGET Physics PROPERTY CXX_STANDARD 11)
set_property(TARGET Physics PROPERTY CXX_STANDARD_REQUIRED ON)
