#include "GlmConversion.hpp"

namespace Physics {

    btVector3 glmToBt(glm::vec3 const& vec) {
        return btVector3(vec.x, vec.y, vec.z);
    }

    glm::vec3 btToGlm(btVector3 const& vec) {
        return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
    }

}
