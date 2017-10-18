#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace Animation {
    /// An animation loaded from a file.
    /// @todo Implement this class.
    class Skeleton {
        public:
            struct BindPoseBone {
                /// 

                void Save(std::ofstream file) {

                }

                void Load(std::ifstream file) {

                }
            };

            struct BindPose {


                void Save(std::ofstream file) {

                }

                void Load(std::ifstream file) {

                }
            };
            void Load(const std::string& name);

            std::string name;
        private:

    };
}
