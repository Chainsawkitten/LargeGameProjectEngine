#pragma once

#include "../FileSelector.hpp"
#include "Util/AssetConverter.hpp"
#include "../../Resources.hpp"

namespace Animation {
    class Skeleton;
}

namespace GUI {
    /// Editor for skeleton.
    class SkeletonEditor {
        public:
            /// Constructor.
            SkeletonEditor();

            /// Show the editor.
            void Show();

            /// Returns the active skeleton.
            /**
             * @return Active skeleton.
             */
            Animation::Skeleton* GetSkeleton();

            /// Set the skeleton to edit.
            /**
             * @param folder Resource folder containing the skeleton.
             * @param model Skeleton to edit.
             */
            void SetSkeleton(ResourceList::ResourceFolder* folder, Animation::Skeleton* skeleton);
            
            /// Get whether the window is visable.
            /**
             * @return True if visable.
             */
            bool IsVisible() const;

            /// Set whether the window should be visable.
            /**
             * @param visable Whether the window should be visible.
             */
            void SetVisible(bool visable);

        private:
            void FileSelected(const std::string& file);

            ResourceList::ResourceFolder* folder = nullptr;
            Animation::Skeleton* skeleton = nullptr;
            bool visible = false;

            FileSelector fileSelector;

            char name[128];
    };
}