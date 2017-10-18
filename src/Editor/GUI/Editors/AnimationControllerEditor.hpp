#pragma once

#include "NodeEditor.hpp"

namespace Animation {
    class AnimationClip;
    class AnimationController;
    class Skeleton;
}

/// Animation controller, node editor class.
class AnimationControllerEditor : public NodeEditor {
    public:
        /// Create new animation controller editor editor.
        AnimationControllerEditor();

        /// Destructor.
        ~AnimationControllerEditor();

        /// Set the animation controller you wish to edit.
        /**
         * @param animationController Animation controller the user wishes to edit.
         */
        void SetAnimationController(Animation::AnimationController* animationController);

        /// Returns the active animation controller.
        /**
         * @return The active animation controller.
         */
        Animation::AnimationController* Get();

    protected:
        /// Show the context menu.
        virtual void ShowContextMenu() override;

        /// Shows the animation node.
        /**
         * @param node Node to be shown.
         */
        virtual void ShowNode(Node * node) override;

        /// Return the a node array of animation related nodes.
        /**
         * @return The node array.
         */
        virtual Node** GetNodeArray() override;

        /// Return the number of animation nodes in the array.
        /**
         * @return The number of animation nodes.
         */
        virtual unsigned int GetNumNodes() override;

        /// Returns true if the two nodes can connect.
        /**
         * @param output The output node.
         * @param input The input node.
         */
        virtual bool CanConnect(Node * output, Node * input) override;

    private:
        Animation::AnimationController* animationController;
};