#include "AssetConverterSkeleton.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Engine/Geometry/MathFunctions.hpp>
#include <Utility/Log.hpp>

AssetConverterSkeleton::AssetConverterSkeleton() {
}

AssetConverterSkeleton::~AssetConverterSkeleton() {
}

bool AssetConverterSkeleton::Convert(const char * filepath, const char * destination,
    bool isSkeleton) {

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
    for (int i = 0; i < aScene->mAnimations[0]->mNumChannels; ++i)
        bones.push_back(aScene->mAnimations[0]->mChannels[i]->mNodeName.C_Str());

    currentId = -1;
    BoneRecursive(aScene->mRootNode, -1);

    if (isSkeleton) {
        Animation::Skeleton skeleton;
        for (unsigned int i = 0; i < aScene->mAnimations[0]->mNumChannels; ++i) {
            aiNodeAnim* channel = aScene->mAnimations[0]->mChannels[i];
            Animation::Skeleton::SkeletonBone * bone = new Animation::Skeleton::SkeletonBone;
            bone->parentId = (uint32_t)parents[i];
            Log() << parents[i] << "\n";
            // Rotation, local.
            glm::quat rot;
            rot.x = channel->mRotationKeys[0].mValue.x;
            rot.y = channel->mRotationKeys[0].mValue.y;
            rot.z = channel->mRotationKeys[0].mValue.z;
            rot.w = channel->mRotationKeys[0].mValue.w;

            glm::mat4 modelMatrix(1.0f);
            glm::vec3 pos;

            pos.x = channel->mPositionKeys[0].mValue.x;
            pos.y = channel->mPositionKeys[0].mValue.y;
            pos.z = channel->mPositionKeys[0].mValue.z;
            modelMatrix = glm::translate(modelMatrix, pos);

            bone->localTx = modelMatrix;

            skeleton.skeletonBones.push_back(bone);
        }

        // Save to file.
        skeleton.Save((std::string(destination)).c_str());
    } else {
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
                anim.bones[i].rotationKeys[j] = channel->mRotationKeys[j].mTime;

                glm::quat rot;
                rot.x = channel->mRotationKeys[j].mValue.x;
                rot.y = channel->mRotationKeys[j].mValue.y;
                rot.z = channel->mRotationKeys[j].mValue.z;
                rot.w = channel->mRotationKeys[j].mValue.w;

                anim.bones[i].rotations[j] = glm::mat4(rot);
            }
        }

        std::ofstream file((std::string(destination)).c_str(), std::ios::binary);
        anim.Save(&file);
        file.close();
    }

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
