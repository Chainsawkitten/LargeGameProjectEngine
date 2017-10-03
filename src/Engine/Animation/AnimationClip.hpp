#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Animation {
    /// An animation loaded from a file.
    class AnimationClip {
        public:
            /// Dock
            struct Bone {
                uint32_t parent;
                uint32_t numRotationKeys;
                int32_t* rotationKeys = nullptr;
                glm::mat4* rotations = nullptr;

                ~Bone() {
                }

                void Save(std::ofstream * file) {
                    file->write(reinterpret_cast<char*>(&parent), sizeof(uint32_t));
                    file->write(reinterpret_cast<char*>(&numRotationKeys), sizeof(uint32_t));
                    file->write(reinterpret_cast<char*>(rotationKeys), sizeof(int32_t) * numRotationKeys);
                    file->write(reinterpret_cast<char*>(rotations), sizeof(glm::mat4) * numRotationKeys);
                }

                void Load(std::ifstream * file) {
                    file->read(reinterpret_cast<char*>(&parent), sizeof(uint32_t));
                    file->read(reinterpret_cast<char*>(&numRotationKeys), sizeof(uint32_t));

                    if (rotationKeys != nullptr)
                        delete[] rotationKeys;

                    rotationKeys = new int32_t[numRotationKeys];
                    file->read(reinterpret_cast<char*>(rotationKeys), sizeof(int32_t) * numRotationKeys);


                    if (rotationKeys != nullptr) 
                        delete[] rotationKeys;

                    rotations = new glm::mat4[numRotationKeys];
                    file->read(reinterpret_cast<char*>(rotations), sizeof(glm::mat4) * numRotationKeys);
                }
            };

            struct Animation {
                uint32_t numBones;
                Bone* bones = nullptr;
                
                void Save(std::ofstream * file) {
                    file->write(reinterpret_cast<char*>(&numBones), sizeof(uint32_t));
                    file->write(reinterpret_cast<char*>(bones), sizeof(Bone) * numBones);
                }

                void Load(std::ifstream * file) {
                    file->read(reinterpret_cast<char*>(&numBones), sizeof(uint32_t));

                    if (bones != nullptr)
                        delete[] bones;
                    bones = new Bone[numBones];
                    file->read(reinterpret_cast<char*>(bones), sizeof(Bone) * numBones);
                }
            };


            void Load(const std::string& name);

            std::string name;
        private:

    };
}
