#include "AssetConverterSkeleton.hpp"
#include <glm/gtc/quaternion.hpp>
#include <Engine/Geometry/MathFunctions.hpp>
#include <Utility/Log.hpp>

AssetConverterSkeleton::AssetConverterSkeleton() {
}

AssetConverterSkeleton::~AssetConverterSkeleton() {
}

bool AssetConverterSkeleton::Convert(const char * filepath, const char * destination,
    bool bindpose) {

    success = true;
    errorString = "";

    const aiScene* aScene = aImporter.ReadFile(filepath, 0);

    bones.clear();
    bones.shrink_to_fit();

    if (!aScene->HasAnimations()) {
        errorString.append("Error: No animation in scene.");
        success = false;
        return false;
    }

    // Push back the bones.
    for (int i = 0; i < aScene->mAnimations[0]->mNumChannels; ++i) {
        bones.push_back(aScene->mAnimations[0]->mChannels[i]->mNodeName.C_Str());
    }

    currentId = -1;
    BoneRecursive(aScene->mRootNode, -1);

    Animation::AnimationClip::Animation anim;

    anim.numBones = aScene->mAnimations[0]->mNumChannels;
    anim.bones = new Animation::AnimationClip::Bone[aScene->mAnimations[0]->mNumChannels];

    for (int i = 0; i < aScene->mAnimations[0]->mNumChannels; ++i) {
        aiNodeAnim* channel = aScene->mAnimations[0]->mChannels[i];
        anim.bones[i].parent = (uint32_t)parents[i];
        anim.bones[i].numRotationKeys = channel->mNumRotationKeys;
        anim.bones[i].rotationKeys = new int32_t[anim.bones[i].numRotationKeys];
        anim.bones[i].rotations = new glm::mat4[anim.bones[i].numRotationKeys];
        
        for (int j = 0; j < channel->mNumRotationKeys; ++j) {
            anim.bones[i].rotationKeys[j] = channel->mRotationKeys->mTime;
            glm::quat rot;
            rot.x = channel->mRotationKeys->mValue.x;
            rot.y = channel->mRotationKeys->mValue.y;
            rot.z = channel->mRotationKeys->mValue.z;
            rot.w = channel->mRotationKeys->mValue.w;

            anim.bones[i].rotations[j] = glm::mat4(rot);
        }
    }

    std::ofstream file((std::string(destination) + ".asset").c_str(), std::ios::binary);
    anim.Save(&file);
    file.close();
    return false;
}

bool AssetConverterSkeleton::Success() const {
    return false;
}

std::string & AssetConverterSkeleton::GetErrorString() {
    return errorString;
}

void AssetConverterSkeleton::BoneRecursive(aiNode * node, int parent) {
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        aiNode * child = node->mChildren[i];

        for (unsigned int j = 0; j < bones.size(); ++j) {
            if (child->mName.C_Str() == bones[j]) {
                children.push_back(child->mName.C_Str());
                parents.push_back(parent);
                ++currentId;
                ids.push_back(currentId);
                break;
            }
        }
        BoneRecursive(child, currentId);
    }
}
