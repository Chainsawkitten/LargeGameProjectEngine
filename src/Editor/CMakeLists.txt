set(SRCS
        main.cpp
        Editor.cpp
        Resources.cpp
        ImGui/OpenGLImplementation.cpp
        ImGui/Splitter.cpp
        ImGui/Theme.cpp
        ImGui/GuiHelpers.cpp
        GUI/FiltersWindow.cpp
        GUI/SelectHymnWindow.cpp
        GUI/SavePromptWindow.cpp
        GUI/InputWindow.cpp
        GUI/FileSelector.cpp
        GUI/ResourceView.cpp
        GUI/SettingsWindow.cpp
        GUI/Editors/EntityEditor.cpp
        GUI/Editors/ModelEditor.cpp
        GUI/Editors/SceneEditor.cpp
        GUI/Editors/SoundEditor.cpp
        GUI/Editors/ScriptEditor.cpp
        GUI/Editors/TextureEditor.cpp
        Util/AssetConverter.cpp
        Util/AssetMetaData.cpp
        Util/EditorSettings.cpp
    )

set(HEADERS
        Editor.hpp
        Resources.hpp
        ImGui/OpenGLImplementation.hpp
        ImGui/Splitter.hpp
        ImGui/Theme.hpp
        ImGui/GuiHelpers.hpp
        GUI/FiltersWindow.hpp
        GUI/SelectHymnWindow.hpp
        GUI/SavePromptWindow.hpp
        GUI/InputWindow.hpp
        GUI/FileSelector.hpp
        GUI/ResourceView.hpp
        GUI/SettingsWindow.hpp
        GUI/Editors/EntityEditor.hpp
        GUI/Editors/ModelEditor.hpp
        GUI/Editors/SceneEditor.hpp
        GUI/Editors/SoundEditor.hpp
        GUI/Editors/ScriptEditor.hpp
        GUI/Editors/TextureEditor.hpp
        Util/AssetConverter.hpp
        Util/AssetMetaData.hpp
        Util/EditorSettings.hpp
    )

create_directory_groups(${SRCS} ${HEADERS})

add_executable(Editor ${SRCS} ${HEADERS})
target_link_libraries(Editor Engine)
set_property(TARGET Editor PROPERTY CXX_STANDARD 11)
set_property(TARGET Editor PROPERTY CXX_STANDARD_REQUIRED ON)
