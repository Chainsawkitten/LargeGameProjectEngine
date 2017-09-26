set(SRCS
        DebugDrawing.cpp
        FrameBuffer.cpp
        ParticleRenderer.cpp
        ReadWriteTexture.cpp
        Renderer.cpp
        RenderSurface.cpp
        Culling/AxisAlignedBoundingBox.cpp
        Culling/Frustum.cpp
        Geometry/Geometry2D.cpp
        Geometry/Geometry3D.cpp
        Geometry/Rectangle.cpp
        Geometry/VertexType/SkinVertex.cpp
        Geometry/VertexType/StaticVertex.cpp
        Lighting/Lighting.cpp
        PostProcessing/ColorFilter.cpp
        PostProcessing/FogFilter.cpp
        PostProcessing/FXAAFilter.cpp
        PostProcessing/GammaCorrectionFilter.cpp
        PostProcessing/GlowBlurFilter.cpp
        PostProcessing/GlowFilter.cpp
        PostProcessing/PostProcessing.cpp
        RenderProgram/SkinRenderProgram.cpp
        RenderProgram/StaticRenderProgram.cpp
        Shader/Shader.cpp
        Shader/ShaderProgram.cpp
        Texture/Texture.cpp
        Texture/Texture2D.cpp
    )

set(HEADERS
        DebugDrawing.hpp
        FrameBuffer.hpp
        ParticleRenderer.hpp
        ReadWriteTexture.hpp
        Renderer.hpp
        RenderSurface.hpp
        Culling/AxisAlignedBoundingBox.hpp
        Culling/Frustum.hpp
        Geometry/Geometry2D.hpp
        Geometry/Geometry3D.hpp
        Geometry/Rectangle.hpp
        Geometry/VertexType/SkinVertex.hpp
        Geometry/VertexType/StaticVertex.hpp
        Lighting/Light.hpp
        Lighting/Lighting.hpp
        PostProcessing/ColorFilter.hpp
        PostProcessing/Filter.hpp
        PostProcessing/FogFilter.hpp
        PostProcessing/FXAAFilter.hpp
        PostProcessing/GammaCorrectionFilter.hpp
        PostProcessing/GlowBlurFilter.hpp
        PostProcessing/GlowFilter.hpp
        PostProcessing/PostProcessing.hpp
        RenderProgram/SkinRenderProgram.hpp
        RenderProgram/StaticRenderProgram.hpp
        Shader/Shader.hpp
        Shader/ShaderProgram.hpp
        Texture/Texture.hpp
        Texture/Texture2D.hpp
    )

# Setup embedded source
set(EMBEDDED_INPUT
    ../../shaders/DebugDrawing.vert
    ../../shaders/DebugDrawing.frag
    ../../shaders/Default3D.vert
    ../../shaders/Default3D.frag
    ../../shaders/Deferred.frag
    ../../shaders/EditorEntity.vert
    ../../shaders/EditorEntity.geom
    ../../shaders/EditorEntity.frag
    ../../shaders/Particle.vert
    ../../shaders/Particle.geom
    ../../shaders/Particle.frag
    ../../shaders/Post.vert
    ../../shaders/PostColor.frag
    ../../shaders/PostCopy.frag
    ../../shaders/PostDither.frag
    ../../shaders/PostFog.frag
    ../../shaders/PostFXAA.frag
    ../../shaders/PostGamma.frag
    ../../shaders/PostGlow.frag
    ../../shaders/PostGlowBlur.frag
    ../../shaders/Skinning.vert
)

foreach(item ${EMBEDDED_INPUT})
    ProcessWithHeaderize(EMBEDDED_HEADER EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set_property(SOURCE ${SRCS} APPEND PROPERTY OBJECT_DEPENDS ${EMBEDDED_HEADER})

create_directory_groups(${SRCS} ${HEADERS})

add_library(Video STATIC ${SRCS} ${HEADERS} ${EMBEDDED_SOURCE})
target_link_libraries(Video Utility glew glm assimp jsoncpp_lib_static stb ${OpenVR_LIBRARY})
set_property(TARGET Video PROPERTY CXX_STANDARD 11)
set_property(TARGET Video PROPERTY CXX_STANDARD_REQUIRED ON)
