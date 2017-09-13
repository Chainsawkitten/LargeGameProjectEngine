set(SRCS
    GlmConversion.cpp
    RigidBody.cpp
    Simulator.cpp
)

set(HEADERS
    GlmConversion.hpp
    RigidBody.hpp
    Simulator.hpp
)

create_directory_groups(${SRCS} ${HEADERS})

add_library(Physics STATIC ${SRCS} ${HEADERS})
target_link_libraries(Physics glm bullet)
set_property(TARGET Physics PROPERTY CXX_STANDARD 11)
set_property(TARGET Physics PROPERTY CXX_STANDARD_REQUIRED ON)
