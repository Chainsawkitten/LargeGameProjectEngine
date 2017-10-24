#pragma once

#include <glm/glm.hpp>
#include <openvr.h>
#include "../linking.hpp"

/// Handles communication with VR devices using OpenVR.
class VRManager {
    friend class Hub;

    public:
    /// Check if the VRManger is active.
    /**
         * @return Whether hmd is present or not.
         */
    ENGINE_API bool Active() const;

    /// Sync VR device pose(s).
    ENGINE_API void Sync();

    /// Recommended render target size to minimize pixel stretching.
    /**
         * @return Recommended size (in pixels).
         */
    ENGINE_API glm::vec2 GetRecommendedRenderTargetSize() const;

    /// Returns the transform for the HMD.
    /**
         * @return The HMD translation matrix.
         */
    ENGINE_API glm::mat4 GetHMDPoseMatrix() const;

    /// Returns the transform for the controllers
    /**
         * @param controlID Which controller to get the matrix for (1 = left, 2 = right).
         * @return The Controllers translation matrix.
         */
    ENGINE_API glm::mat4 GetControllerPoseMatrix(int controlID) const;

    /// Returns the transform between the view space and eye space.
    /**
         * @param eye Which eye the function should return the eye matrix for.
         * @return The eye matrix.
         */
    ENGINE_API glm::mat4 GetHMDEyeToHeadMatrix(vr::Hmd_Eye eye) const;

    /// Returns the projection matrix to use for the specified eye.
    /**
         * @param eye Which eye the function should return the eye matrix for.
         * @param zNear Distance to the near clip plane in meters.
         * @param zFar Distance to the far clip plane in meters.
         * @return The projection matrix.
         */
    ENGINE_API glm::mat4 GetHMDProjectionMatrix(vr::Hmd_Eye eye, float zNear, float zFar) const;

    /// Submit a texture to an eye.
    /**
         * @param eye Which eye to submit texture.
         * @param texture Texture to submit.
         */
    ENGINE_API void Submit(vr::Hmd_Eye eye, vr::Texture_t* texture) const;

    /// Get scale of player.
    /**
         * @return Scale to get.
         */
    ENGINE_API float GetScale() const;

    /// Set scale of player.
    /**
         * @param scale Scale to set.
         */
    ENGINE_API void SetScale(float scale);

    /// Gets if certain button is pressed (Only trigger for now)
    /**
         * @param buttonID the button we want to check is pressed.
         * @return whether certain button is pressed or not. 
         */
    ENGINE_API bool GetInput(vr::EVRButtonId buttonID);

    private:
    VRManager();
    ~VRManager();
    VRManager(VRManager const&) = delete;
    void operator=(VRManager const&) = delete;

    static glm::mat4 ConvertMatrix(const vr::HmdMatrix34_t& mat);
    static glm::mat4 ConvertMatrix(const vr::HmdMatrix44_t& mat);

    float scale;
    vr::IVRSystem* vrSystem;
    vr::TrackedDevicePose_t tracedDevicePoseArray[vr::k_unMaxTrackedDeviceCount];
    glm::mat4 deviceTransforms[vr::k_unMaxTrackedDeviceCount];

    bool pressedTrackedDevice[vr::k_unMaxTrackedDeviceCount];
};
