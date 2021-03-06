#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Utility/Log.hpp>
#include "../linking.hpp"

namespace Animation {
    struct Bone;
    struct Animation;
}

namespace Animation {
    /// An animation loaded from a file.
    class AnimationClip {
        public:
            /// Default constructor.
            AnimationClip() = default;

            /// Load.
            /**
             * @param name Name of animation.
             */
            ENGINE_API void Load(const std::string& name);       

            Animation* animation = nullptr;

            /// Path of animation clip.
            std::string path;

            /// Name of animation clip.
            std::string name;
    };
}
