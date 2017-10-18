#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include "../Util/Node.hpp"

namespace Animation {
    class AnimationClip;
    class Skeleton;
}

namespace Animation {
    /// An animation loaded from a file.
    class AnimationController {
        public:
            /// Animaiton action node.
            struct AnimationAction : public Node {
                char animationClip[128];
                float playbackModifier = 1.0f;
                bool repeat = true;

                /// Save the animation action node.
                virtual void Save(std::ofstream* file) override {
                    Node::Save(file);
                    file->write(reinterpret_cast<char*>(&index), sizeof(uint32_t));
                }

                /// Load the animation action node.
                virtual void Load(std::ifstream* file) override {
                    Node::Load(file);
                    file->read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
                }
            };

            /// Animation transition node.
            struct AnimationTransition : public Node {
                uint32_t in;
                float inTransitionValue;
                uint32_t out;
                float outTransitionValue;
                glm::vec2 pos;

                /// Save the animation transition node.
                virtual void Save(std::ofstream* file) override {
                    Node::Save(file);
                    file->write(reinterpret_cast<char*>(&in), sizeof(uint32_t));
                    file->write(reinterpret_cast<char*>(&inTransitionValue), sizeof(float));
                    file->write(reinterpret_cast<char*>(&out), sizeof(uint32_t));
                    file->write(reinterpret_cast<char*>(&outTransitionValue), sizeof(float));
                    file->write(reinterpret_cast<char*>(&pos), sizeof(glm::vec2));
                }

                /// Load the animation transition node.
                virtual void Load(std::ifstream* file) override {
                    Node::Load(file);
                    file->read(reinterpret_cast<char*>(&in), sizeof(uint32_t));
                    file->read(reinterpret_cast<char*>(&inTransitionValue), sizeof(float));
                    file->read(reinterpret_cast<char*>(&out), sizeof(uint32_t));
                    file->read(reinterpret_cast<char*>(&outTransitionValue), sizeof(float));
                    file->read(reinterpret_cast<char*>(&pos), sizeof(glm::vec2));
                }
            };

            /// Save animation controller.
            void Save(const std::string& name);

            /// Load animation controller.
            void Load(const std::string& name);

            /// Vector with the animation nodes.
            std::vector<Node*> animationNodes;

            /// The first active animation.
            AnimationAction* activeAction1 = nullptr;

            /// The second active animation.
            AnimationAction* activeAction2 = nullptr;

            /// Transition between the two animations.
            AnimationTransition* activeTransition = nullptr;

            /// Name of resource.
            std::string name;

            /// The folder containing the model file.
            std::string path;

        private:

    };
}
