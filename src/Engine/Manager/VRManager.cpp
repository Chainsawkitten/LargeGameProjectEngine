#include "VRManager.hpp"
#include "Utility/Log.hpp"

VRManager::VRManager() : scale(1.f) {

    // Load VR Runtime.
    vr::EVRInitError eError = vr::VRInitError_None;
    vrSystem = vr::VR_Init(&eError, vr::VRApplication_Scene);
    if (eError != vr::VRInitError_None)
    {
        vrSystem = nullptr;
        Log() << "Unable to init VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << "\n";
        return;
    }

    // Get focus.
    vr::VRCompositor()->WaitGetPoses(NULL, 0, NULL, 0);
}

VRManager::~VRManager() {
    if (vrSystem == nullptr)
        return;
    
    vr::VR_Shutdown();
    vrSystem = nullptr;
}

bool VRManager::Active() const {
    return vrSystem != nullptr;
}

void VRManager::Sync() {
    if (vrSystem == nullptr) {
        Log() << "No initialized VR device.\n";
        return;
    }

    // Get VR device pose(s).
    vr::VRCompositor()->WaitGetPoses(tracedDevicePoseArray, vr::k_unMaxTrackedDeviceCount, NULL, 0);

    // Convert to glm format.
    for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
        if (tracedDevicePoseArray[nDevice].bPoseIsValid)
            deviceTransforms[nDevice] = ConvertMatrix(tracedDevicePoseArray[nDevice].mDeviceToAbsoluteTracking);
}

glm::vec2 VRManager::GetRecommendedRenderTargetSize() const {
    if (vrSystem == nullptr) {
        Log() << "No initialized VR device.\n";
        return glm::vec2();
    }

    std::uint32_t width, height;
    vrSystem->GetRecommendedRenderTargetSize(&width, &height);

    return glm::vec2(width, height);
}

glm::mat4 VRManager::GetHMDPoseMatrix() const {
    if (vrSystem == nullptr) {
        Log() << "No initialized VR device.\n";
        return glm::mat4();
    }

    return glm::inverse(deviceTransforms[vr::k_unTrackedDeviceIndex_Hmd]);
}

glm::mat4 VRManager::GetHMDEyeToHeadMatrix(vr::Hmd_Eye eye) const {
    if (vrSystem == nullptr) {
        Log() << "No initialized VR device.\n";
        return glm::mat4();
    }

    return glm::inverse(ConvertMatrix(vrSystem->GetEyeToHeadTransform(eye)));
}

glm::mat4 VRManager::GetHMDProjectionMatrix(vr::Hmd_Eye eye, float zNear, float zFar) const {
    if (vrSystem == nullptr) {
        Log() << "No initialized VR device.\n";
        return glm::mat4();
    }

    return ConvertMatrix(vrSystem->GetProjectionMatrix(eye, zNear, zFar));
}

void VRManager::Submit(vr::Hmd_Eye eye, vr::Texture_t* texture) const {
    if (vrSystem == nullptr) {
        Log() << "No initialized VR device.\n";
        return;
    }

    const vr::EVRCompositorError eError = vr::VRCompositor()->Submit(eye, texture);
    if (eError != vr::VRCompositorError_None)
        Log() << "Unable to submit texture to hmd: " << eError << "\n";
}

glm::mat4 VRManager::ConvertMatrix(const vr::HmdMatrix34_t& mat)
{
    glm::mat4 glmMat(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0,
        mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0,
        mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0,
        mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f
    );
    return glmMat;
}

glm::mat4 VRManager::ConvertMatrix(const vr::HmdMatrix44_t& mat)
{
    glm::mat4 glmMat(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
    return glmMat;
}

float VRManager::GetScale() const {
    return scale;
}

void VRManager::SetScale(float scale) {
    this->scale = scale;
}
