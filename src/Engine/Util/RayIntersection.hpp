#pragma once

#include "../Geometry/AssetFileHandler.hpp"
#include "../Geometry/Model.hpp"
#include "../linking.hpp"

/// The class handling intersection checks.
class RayIntersection {
    public:
    /// Constructor.
    ENGINE_API RayIntersection();

    /// Destructor.
    ENGINE_API ~RayIntersection();

    /// Checks whether a ray intersects with an OBB.
    /**
         * @param rayOrigin The position the ray originates from.
         * @param rayDirection The direction the ray is shooting towards.
         * @param meshData The bounding box belonging to the mesh that is tested against.
         * @param modelMatrix The model matrix belonging to the mesh that is tested against.
         * @param outputDistance The distance to the entity that is tested to be used as a return value check.
         * @return Whether or not the ray intersects with the entity that is tested against.
         */
    ENGINE_API bool RayOBBIntersect(glm::vec3 rayOrigin,
        glm::vec3 rayDirection,
        Video::AxisAlignedBoundingBox meshData,
        glm::mat4 modelMatrix,
        float& outputDistance) const;
};
