set(SRCS
        Buffer/FrameBuffer.cpp
        Buffer/ReadWriteTexture.cpp
        Buffer/StorageBuffer.cpp
        DebugDrawing.cpp  
        ParticleRenderer.cpp
        Renderer.cpp
        RenderSurface.cpp
        Culling/AxisAlignedBoundingBox.cpp
        Culling/Frustum.cpp
        Geometry/Geometry2D.cpp
        Geometry/Geometry3D.cpp
        Geometry/Rectangle.cpp
        Geometry/VertexType/SkinVertex.cpp
        Geometry/VertexType/StaticVertex.cpp
        PostProcessing/FXAAFilter.cpp
        PostProcessing/PostProcessing.cpp
        Profiling/Query.cpp
        RenderProgram/RenderProgram.cpp
        RenderProgram/SkinRenderProgram.cpp
        RenderProgram/StaticRenderProgram.cpp
        Shader/Shader.cpp
        Shader/ShaderProgram.cpp
        ShadowPass.cpp
        Texture/Texture2D.cpp
        Texture/TextureHCT.cpp
        Texture/TexturePNG.cpp
        VideoErrorCheck.cpp
    )

set(HEADERS
        linking.hpp
        Buffer/FrameBuffer.hpp
        Buffer/ReadWriteTexture.hpp
        Buffer/StorageBuffer.hpp
        DebugDrawing.hpp
        ParticleRenderer.hpp
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
        PostProcessing/Filter.hpp
        PostProcessing/FXAAFilter.hpp
        PostProcessing/PostProcessing.hpp
        Profiling/Query.hpp
        RenderProgram/RenderProgram.hpp
        RenderProgram/SkinRenderProgram.hpp
        RenderProgram/StaticRenderProgram.hpp
        Shader/Shader.hpp
        Shader/ShaderProgram.hpp
        ShadowPass.hpp
        Texture/Texture2D.hpp
        Texture/TextureHCT.hpp
        Texture/TexturePNG.hpp
        VideoErrorCheck.hpp
    )

# Setup embedded source
set(EMBEDDED_INPUT
    ../../shaders/DebugDrawing.vert
    ../../shaders/DebugDrawing.frag
    ../../shaders/Default3D.vert
    ../../shaders/Default3D.frag
    ../../shaders/EditorEntity.vert
    ../../shaders/EditorEntity.geom
    ../../shaders/EditorEntity.frag
    ../../shaders/Particle.vert
    ../../shaders/Particle.geom
    ../../shaders/Particle.frag
    ../../shaders/Post.vert
    ../../shaders/PostFXAA.frag
    ../../shaders/ShadowSkin.vert
    ../../shaders/ShadowStatic.vert
    ../../shaders/Skinning.vert
    ../../shaders/Zrejection.frag
    ../../shaders/ZrejectionSkin.vert
    ../../shaders/ZrejectionStatic.vert
)

foreach(item ${EMBEDDED_INPUT})
    ProcessWithHeaderize(EMBEDDED_HEADER EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set_property(SOURCE ${SRCS} APPEND PROPERTY OBJECT_DEPENDS ${EMBEDDED_HEADER})

create_directory_groups(${SRCS} ${HEADERS})

add_library(Video SHARED ${SRCS} ${HEADERS} ${EMBEDDED_SOURCE})
target_link_libraries(Video Utility glew glm assimp jsoncpp_lib stb ${OpenVR_LIBRARY})

if(UseMemTrack)
    target_link_libraries(Video memtrack memtracknew)
endif()

set_property(TARGET Video PROPERTY CXX_STANDARD 11)
set_property(TARGET Video PROPERTY CXX_STANDARD_REQUIRED ON)
