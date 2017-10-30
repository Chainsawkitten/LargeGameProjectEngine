#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include "../linking.hpp"

namespace Animation {
    /// An animation loaded from a file.
    class Skeleton {
        public:
            struct SkeletonBone {
                glm::mat4 localTx;
                glm::mat4 globalTx;
                glm::mat4 inversed;
                uint32_t parentId;

                void Save(std::ofstream * file) {
                    file->write(reinterpret_cast<char*>(&localTx), sizeof(glm::mat4));
                    file->write(reinterpret_cast<char*>(&globalTx), sizeof(glm::mat4));
                    file->write(reinterpret_cast<char*>(&inversed), sizeof(glm::mat4));
                    file->write(reinterpret_cast<char*>(&parentId), sizeof(uint32_t));
                }

                void Load(std::ifstream * file) {
                    file->read(reinterpret_cast<char*>(&localTx), sizeof(glm::mat4));
                    file->read(reinterpret_cast<char*>(&globalTx), sizeof(glm::mat4));
                    file->read(reinterpret_cast<char*>(&inversed), sizeof(glm::mat4));
                    file->read(reinterpret_cast<char*>(&parentId), sizeof(uint32_t));
                }

            };

            /// Constructor.
            ENGINE_API Skeleton();

            /// Save skeleton.
            /**
             * @param name Name of file.
             */
            ENGINE_API void Save(std::string name);

            /// Load skeleton.
            /** 
             * @param name Name of skeleton to load.
             */ 
            ENGINE_API void Load(std::string name);

            /// Bones of skeleton.
            std::vector<SkeletonBone*> skeletonBones;

            /// Path to skeleton.
            std::string path;

            /// Name of the resource.
            std::string name;

        private:
            void InitSkeleton();
    };
}
