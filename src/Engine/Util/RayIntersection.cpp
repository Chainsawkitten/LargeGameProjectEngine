#include "RayIntersection.hpp"

RayIntersection::RayIntersection() {
    
}

RayIntersection::~RayIntersection() {
    
}

bool RayIntersection::RayOBBIntersect(glm::vec3 rayOrigin, glm::vec3 rayDirection, Video::AxisAlignedBoundingBox meshData, glm::mat4 modelMatrix, float &outputDistance) {

    float tMin = -INFINITY;
    float tMax = INFINITY;
  
    glm::vec3 worldPos = glm::vec3(modelMatrix[3].x, modelMatrix[3].y, modelMatrix[3].z);
    glm::vec3 delta = worldPos - rayOrigin;

    float minValue[3] = { meshData.minVertex.x, meshData.minVertex.y, meshData.minVertex.z };
    float maxValue[3] = { meshData.maxVertex.x, meshData.maxVertex.y, meshData.maxVertex.z };

    for (int i = 0; i < 3; i++) {

        glm::vec3 currentAxis = glm::vec3(modelMatrix[i].x, modelMatrix[i].y, modelMatrix[i].z);
        float e = glm::dot(currentAxis, delta);
        float f = glm::dot(rayDirection, currentAxis);

        if (fabs(f) > pow(10, -10)) {

            float t1 = (e + minValue[i]) / f;
            float t2 = (e + maxValue[i]) / f;

            if (t1 > t2) {
                float w = t1; t1 = t2; t2 = w;
            }

            if (t2 < tMax)
                tMax = t2;

            if (t1 > tMin)
                tMin = t1;

            if (tMax < tMin)
                return false;

            if (tMax < 0)
                return false;

        } else {
            if (-e + minValue[i] > 0.0f || -e + maxValue[i] < 0.0f)
                return false;
        }
    }

    if (tMin > 0)
        outputDistance = tMin;
    else
        outputDistance = tMax;
    return true;
}
