#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include "../Util/Node.hpp"
#include <map>
#include "../linking.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include <Utility/Log.hpp>
#include <map>

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
                char animationClipName[128];
                float playbackModifier = 1.0f;
                bool repeat = true;
                Animation::AnimationClip * animationClip = nullptr;

                virtual ~AnimationAction() {
                    if (animationClip != nullptr)
                        Managers().resourceManager->FreeAnimationClip(animationClip);
                }

                /// Save the animation action node.
                /**
                 * @param file File to save to.
                 */
                virtual void Save(std::ofstream* file) override {
                    Node::Save(file);
                    file->write(reinterpret_cast<char*>(animationClipName), 128);
                    file->write(reinterpret_cast<char*>(&playbackModifier), sizeof(uint32_t));
                    file->write(reinterpret_cast<char*>(&repeat), sizeof(bool));
                }

                /// Load the animation action node.
                /**
                 * @param file File to load from.
                 */
                virtual void Load(std::ifstream* file) override {
                    Node::Load(file);
                    file->read(reinterpret_cast<char*>(animationClipName), 128);
                    file->read(reinterpret_cast<char*>(&playbackModifier), sizeof(uint32_t));
                    file->read(reinterpret_cast<char*>(&repeat), sizeof(bool));

                    if (animationClip != nullptr)
                        Managers().resourceManager->FreeAnimationClip(animationClip);

                    animationClip = Managers().resourceManager->CreateAnimationClip(animationClipName);
                }
            };

            /// Animation transition node.
            struct AnimationTransition : public Node {
                float transitionTime;

                /// Save the animation transition node.
                /**
                 * @param file File to save to.
                 */
                virtual void Save(std::ofstream* file) override {
                    Node::Save(file);
                    file->write(reinterpret_cast<char*>(&transitionTime), sizeof(float));
                }

                /// Load the animation transition node.
                /**
                 * @param file File to load from.
                 */
                virtual void Load(std::ifstream* file) override {
                    Node::Load(file);
                    file->read(reinterpret_cast<char*>(&transitionTime), sizeof(float));
                }
            };

            AnimationController() = default;
            ~AnimationController();

            /// Save animation controller.
            ENGINE_API void Save(const std::string& path);

            /// Load animation controller.
            ENGINE_API void Load(const std::string& name);

            /// Vector with the animation nodes.
            std::vector<Node*> animationNodes;

            /// Skeleton.
            Skeleton* skeleton;

            /// Name of resource.
            std::string name;

            /// The folder containing the model file.
            std::string path;

            /// Map of bools.
            std::map<std::string, bool> bools;
    
            /// Map of floats.
            std::map<std::string, float> floats;

    private:
            enum NodeType {
                ACTION = 0,
                TRANSITION = 1
            };


            void Clear();
    };
}
