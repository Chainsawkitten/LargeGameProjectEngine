#pragma once
#include <sstream>
#include <imgui.h>

namespace GUI {
    class LogView {
        public:
        LogView();
        ~LogView();

        void Show();

        bool IsVisible();
        
        private:
        std::stringstream defaultStringstream;
        std::stringstream infoStringstream;
        std::stringstream warningStringstream;
        std::stringstream errorStringstream;

        ImGuiTextBuffer textBuffer;
        ImGuiTextFilter textFilter;
        ImVector<int> lineOffsets;

        bool visible = true;

        static const int splitterSize = 2;
        int logHeight = 250;
        bool logResize = false;

        int sceneWidth = 250;
        bool sceneResize = false;

        int editorWidth = 250;
        bool editorResize = false;
    };
}