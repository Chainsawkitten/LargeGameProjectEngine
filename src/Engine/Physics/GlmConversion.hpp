#pragma once

#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Physics {

    /// Convert GLM vector to Bullet vector.
    /**
     * @param vec Vector to convert.
     * @return Converted vector.
     */
    btVector3 glmToBt(glm::vec3 const& vec);

    /// Convert Bullet vector to GLM vector.
    /**
     * @param vec Vector to convert.
     * @return Converted vector.
     */
    glm::vec3 btToGlm(btVector3 const& vec);

    /// Convert Bullet quaternion to GLM quaternion.
    /**
     * @param quat Quaternion to convert.
     * @return Converted quaternion.
     */
    glm::quat btToGlm(btQuaternion const& quat);
}
