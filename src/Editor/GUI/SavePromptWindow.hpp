#pragma once

#include <functional>
#include <vector>
#include <string>

namespace GUI {
    /// A window that asks the user if they want to save before quitting.
    class SavePromptWindow {
        public:
            
            /// Show the window and let the user decide if they want to save.
            void Show();
            
            /// Get whether the window is visible.
            /**
             * @return Whether the window is visible.
             */
            bool IsVisible() const;
            
            /// Set whether the window should be visible.
            /**
             * @param visible Whether the window should be visible.
             */
            void SetVisible(bool visible);
            
            /// Fetches what the player picked.
            /**
             * @return 0: Save, 1: Don't save, *2: cancel.*
             */
            int GetDecision() const;

        private:

            bool visible = false;
            int decision = -1;
            
    };
}
