#pragma once

#include "Editors/ModelEditor.hpp"
#include "Editors/SceneEditor.hpp"
#include "Editors/SoundEditor.hpp"
#include "Editors/ScriptEditor.hpp"
#include "Editors/TextureEditor.hpp"
#include "GUI/SavePromptWindow.hpp"

namespace GUI {
    /// Displays all the hymn's resources.
    class ResourceView {
        public:
            /// Constructor.
            ResourceView();

            /// Show the resource list.
            void Show();
            
            /// Get whether the resource list is visible.
            /**
             * @return Whether the resource list is visible.
             */
            bool IsVisible() const;
            
            /// Set whether the resource list should be visible.
            /**
             * @param visible Whether the resource list should be visible.
             */
            void SetVisible(bool visible);
            
            /// Hide all editors.
            /**
             * Needs to be called before playing the game or old editors with stale pointers could be shown when returning to the editor.
             */
            void HideEditors();
            
            /// Save the currently active scene.
            void SaveScene() const;
            
            /// Reset which scene is open.
            void ResetScene();

            /// Return Scene Editor
            /**
             * @return The scene editor
             */
            SceneEditor& GetScene();
            
        private:
            bool visible = false;
            
            ScriptEditor scriptEditor;
            SceneEditor sceneEditor;
            ModelEditor modelEditor;
            TextureEditor textureEditor;
            SoundEditor soundEditor;

            GUI::SavePromptWindow savePromptWindow;

            bool changeScene = false;
            int sceneIndex = -1;

            static const int splitterSize = 2;
            int resourceHeight = 250;
            bool resourceResize = false;
            
            int sceneWidth = 250;
            bool sceneResize = false;
            
            int editorWidth = 250;
            bool editorResize = false;
    };
}
