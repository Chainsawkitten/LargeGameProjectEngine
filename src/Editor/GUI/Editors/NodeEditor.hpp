#pragma once

#include <imgui.h>
#include <Engine/Animation/AnimationController.hpp>

/// Generic node editor base class.
class NodeEditor {
    public:
        /// Create new animation controller editor editor.
        NodeEditor();
    
        /// Destructor.
        ~NodeEditor();
    
        /// Show the editor.
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
        void virtual SetVisible(bool visible);
    
    protected:
        /// Show context menu.
        virtual void ShowContextMenu() = 0;

        /// Show node.
        /**
         * @param node Node to be shown.
         */
        virtual void ShowNode(Node* node) = 0;

        ///
        /**
         * @return Array with nodes.
         */
        virtual Node** GetNodeArray() = 0;

        /// Returns the number of nodes in the array.
        /**
         * @return Number of nodes.
         */
        virtual unsigned int GetNumNodes() = 0;


        virtual bool CanConnect(Node * output, Node * input) = 0;

    private:
        void ShowGrid();
    
        ImVec2 scrolling = ImVec2(0.0f, 0.0f);
        ImVec2 nodeWindowPadding = ImVec2(8.0f, 8.0f);

        bool visible = false;    
        bool isDragingConnection = false;
        bool inited = false;
        bool openContextMenu = false;
        bool showGrid = true;

        float nodeSlotRadius = 6.0f;
    
        int hoveredNodeIndex = -1;
        int dragNodeIndex = -1;
        int nodeSelected = -1;
        int nodeHoveredInList = -1;
        int nodeHoveredInScene = -1;
    
};