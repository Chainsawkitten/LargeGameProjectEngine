## Engine ##
set(SRCS
        ../../externals/AngelScript/sdk/add_on/scriptbuilder/scriptbuilder.cpp
        ../../externals/AngelScript/sdk/add_on/scriptmath/scriptmath.cpp
        ../../externals/AngelScript/sdk/add_on/scriptstdstring/scriptstdstring.cpp
        ../../externals/AngelScript/sdk/add_on/scriptstdstring/scriptstdstring_utils.cpp
        ../../externals/imgui/imgui.cpp
        ../../externals/imgui/imgui_demo.cpp
        ../../externals/imgui/imgui_draw.cpp
        Hymn.cpp
        MainWindow.cpp
        Audio/SoundBuffer.cpp
        Audio/VorbisFile.cpp
        Component/Animation.cpp
        Component/DirectionalLight.cpp
        Component/Lens.cpp
        Component/Listener.cpp
        Component/Material.cpp
        Component/Mesh.cpp
        Component/ParticleEmitter.cpp
        Component/Physics.cpp
        Component/PointLight.cpp
        Component/Script.cpp
        Component/SoundSource.cpp
        Component/SpotLight.cpp
        Component/SuperComponent.cpp
        Entity/Entity.cpp
        Entity/World.cpp
        Geometry/Animation.cpp
        Geometry/Cube.cpp
        Geometry/MathFunctions.cpp
        Geometry/Model.cpp
        Geometry/RiggedModel.cpp
        Geometry/Skeleton.cpp
        Geometry/StaticModel.cpp
        Manager/Managers.cpp
        Manager/DebugDrawingManager.cpp
        Manager/ParticleManager.cpp
        Manager/ProfilingManager.cpp
        Manager/PhysicsManager.cpp
        Manager/RenderManager.cpp
        Manager/ResourceManager.cpp
        Manager/ScriptManager.cpp
        Manager/SoundManager.cpp
        Util/FileSystem.cpp
        Util/Input.cpp
        Util/Json.cpp
        Util/Profiling.cpp
        Util/Settings.cpp
        Texture/Texture2D.cpp
        Script/ScriptFile.cpp
        Input/Input.cpp
    )

set(HEADERS
        Hymn.hpp
        MainWindow.hpp
        Audio/SoundBuffer.hpp
        Audio/SoundFile.hpp
        Audio/VorbisFile.hpp
        Component/Animation.hpp
        Component/DirectionalLight.hpp
        Component/Lens.hpp
        Component/Listener.hpp
        Component/Material.hpp
        Component/Mesh.hpp
        Component/ParticleEmitter.hpp
        Component/Physics.hpp
        Component/PointLight.hpp
        Component/Script.hpp
        Component/SoundSource.hpp
        Component/SpotLight.hpp
        Component/SuperComponent.hpp
        Entity/Entity.hpp
        Entity/World.hpp
        Geometry/Animation.hpp
        Geometry/Cube.hpp
        Geometry/MathFunctions.hpp
        Geometry/Model.hpp
        Geometry/RiggedModel.hpp
        Geometry/Skeleton.hpp
        Geometry/StaticModel.hpp
        Manager/Managers.hpp
        Manager/DebugDrawingManager.hpp
        Manager/ParticleManager.hpp
        Manager/ProfilingManager.hpp
        Manager/PhysicsManager.hpp
        Manager/RenderManager.hpp
        Manager/ResourceManager.hpp
        Manager/ScriptManager.hpp
        Manager/SoundManager.hpp
        Util/FileSystem.hpp
        Util/Input.hpp
        Util/Json.hpp
        Util/Profiling.hpp
        Util/Settings.hpp
        Texture/Texture2D.hpp
        Script/ScriptFile.hpp
        Input/Input.hpp
    )

# Setup embedded source
set(EMBEDDED_INPUT
    ../../shaders/Default2D.vert
    ../../shaders/Deferred.frag
    ../../shaders/Texture2D.frag
    ../../shaders/Post.vert
    ../../assets/Camera.png
    ../../assets/DefaultDiffuse.png
    ../../assets/DefaultNormal.png
    ../../assets/DefaultSpecular.png
    ../../assets/DefaultGlow.png
    ../../assets/Light.png
    ../../assets/ParticleAtlas.png
    ../../assets/ParticleEmitter.png
    ../../assets/SoundSource.png
)

foreach(item ${EMBEDDED_INPUT})
    ProcessWithHeaderize(EMBEDDED_HEADER EMBEDDED_SOURCE ${item} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

set_property(SOURCE ${SRCS} APPEND PROPERTY OBJECT_DEPENDS ${EMBEDDED_HEADER})

create_directory_groups(${SRCS} ${HEADERS})

add_library(Engine STATIC ${SRCS} ${HEADERS} ${EMBEDDED_SOURCE})
target_link_libraries(Engine Video imgui stb simpleini ${OPENAL_LIB} angelscript)
set_property(TARGET Engine PROPERTY CXX_STANDARD 11)
set_property(TARGET Engine PROPERTY CXX_STANDARD_REQUIRED ON)
