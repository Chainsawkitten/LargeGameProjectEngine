#pragma once

#include "GUI/SelectHymnWindow.hpp"
#include "GUI/InputWindow.hpp"
#include "GUI/ResourceList.hpp"
#include "GUI/SettingsWindow.hpp"
#include "GUI/FiltersWindow.hpp"
#include "GUI/SavePromptWindow.hpp"
#include <string>
#include <json/json.h>

struct GLFWcursor;

/// Handles the main editor.
class Editor {
    public:
        /// Create new editor.
        Editor();
        
        /// Destructor.
        ~Editor();
        
        /// Show the editor.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Show(float deltaTime);
        
        /// Save the hymn being edited.
        void Save() const;

        /// Loads the save editor state.
        void LoadEditorState();

        /// Is the editor ready to be closed, have we saved everything?
        /**
        * @return Are we ready to close the editor?
        */
        bool ReadyToClose() const;

        /// Close the editor.
        void Close();

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
        
        /// Get the editor camera.
        /**
         * @return Camera through which to render.
         */
        Entity* GetCamera() const;
        
    private:
        void Play();
        void NewHymn();
        void NewHymnClosed(const std::string& hymn);
        void OpenHymn();
        void OpenHymnClosed(const std::string& hymn);
        
        bool visible = true;
        GUI::SelectHymnWindow selectHymnWindow;
        GUI::InputWindow inputWindow;
        GUI::ResourceList resourceList;
        GUI::SettingsWindow settingsWindow;
        GUI::FiltersWindow filtersWindow;
        GUI::SavePromptWindow savePromtWindow;

        bool close;
        bool savePromptAnswered;

        Json::Value editorState;
        
        World cameraWorld;
        Entity* cameraEntity;
        
        double lastX = 0.0;
        double lastY = 0.0;
        
        GLFWcursor* cursors[5];
};
