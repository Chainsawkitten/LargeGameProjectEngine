#pragma once

#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/glm.hpp>
#include "../linking.hpp"

namespace Geometry {
    /// Mix two vectors.
    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @param t Mix factor.
     * @param result Mixed vector.
     */
    ENGINE_API void MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result);

    /// Mix two quaternions.
    /**
     * @param q1 First quaternion.
     * @param q2 Second quaternion.
     * @param t Mix factor.
     * @param result Mixed quaternion.
     */
    ENGINE_API void MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result);

    /// Calculate the dot product of two quaternions.
    /**
     * @param q1 First quaternion.
     * @param q2 Second quaternion.
     * @return t Dot product value.
     */
    ENGINE_API float DotQuat(const glm::quat& q1, const glm::quat& q2);

    /// Normalize quaternion.
    /**
     * @param q Quaternion to normalize.
     */
    ENGINE_API void NormalizeQuat(glm::quat& q);

    /// Create matrix from position, rotation and scale.
    /**
     * @param p Position.
     * @param r Rotation.
     * @param s Scale.
     * @param matrix Transform matrix.
     */
    ENGINE_API void ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& matrix);

    /// Create matrix from quaternion.
    /**
     * @param m Matrix.
     * @param q Quaternion.
     */
    ENGINE_API void QuatToMat(glm::mat4& m, const glm::quat& q);

    /// Convert from assimp quaternion to glm quaternion.
    /**
     * @param glmQuat Glm quaternion.
     * @param aiQuat Assimp quaternion.
     */
    ENGINE_API void CpyQuat(glm::quat& glmQuat, const aiQuaternion& aiQuat);

    /// Convert from assimp vector3 to glm vector3.
    /**
     * @param glmVec Glm vector.
     * @param aiVec Assimp vector.
     */
    ENGINE_API void CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec);

    /// Convert from assimp vector3 to glm vector2.
    /**
     * @param glmVec Glm vector.
     * @param aiVec Assimp vector.
     */
    ENGINE_API void CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec);

    /// Convert from assimp vector2 to glm vector2.
    /**
     * @param glmVec Glm vector.
     * @param aiVec Assimp vector.
     */
    ENGINE_API void CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec);

    /// Convert from assimp matrix4x4 to glm matrix4x4.
    /**
     * @param glmMat Glm matrix.
     * @param aiMat Assimp matrix.
     */
    ENGINE_API void CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat);

    /// Convert from assimp matrix4x4 to assimp matrix3x3.
    /**
     * @param aiMat3 Assimp matrix3x3.
     * @param aiMat4 Assimp matrix4x4.
     */
    ENGINE_API void CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4);
}
