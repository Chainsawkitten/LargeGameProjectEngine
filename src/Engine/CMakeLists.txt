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
        Geometry/AssetFileHandler.cpp
        Geometry/Cube.cpp
        Geometry/MathFunctions.cpp
        Geometry/Model.cpp
        Geometry/Skeleton.cpp
        Manager/Managers.cpp
        Manager/SuperManager.cpp
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
        Texture/TextureAsset.cpp
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
        Geometry/AssetFileHandler.hpp
        Geometry/Cube.hpp
        Geometry/MathFunctions.hpp
        Geometry/Model.hpp
        Geometry/Skeleton.hpp
        Manager/Managers.hpp
        Manager/SuperManager.hpp
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
        Texture/TextureAsset.hpp
        Script/ScriptFile.hpp
        Input/Input.hpp
    )

# Setup embedded source
set(EMBEDDED_INPUT
    ../../assets/Camera.png
    ../../assets/DefaultAlbedo.png
    ../../assets/DefaultNormal.png
    ../../assets/DefaultMetallic.png
    ../../assets/DefaultRoughness.png
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

target_link_libraries(Engine Video Physics catch glfw ${GLFW_LIBRARIES} imgui stb simpleini ${OPENAL_LIB} angelscript SteamAudio)

# Measure VRAM dependencies.
if(MeasureVRAM)
    target_link_libraries(Engine ${DirectX11_LIBRARIES})
    target_include_directories(Engine PRIVATE ${DirectX11_INCLUDE_DIRS})
endif()

# Measure RAM dependencies.
if(WIN32)
    find_library(PSAPI Psapi)
    if(PSAPI)
        add_definitions(-DMEASURE_RAM)
        target_link_libraries(Engine ${PSAPI})
    endif()
endif()

set_property(TARGET Engine PROPERTY CXX_STANDARD 11)
set_property(TARGET Engine PROPERTY CXX_STANDARD_REQUIRED ON)
