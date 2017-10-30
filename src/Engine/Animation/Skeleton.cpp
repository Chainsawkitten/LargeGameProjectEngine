#include "Skeleton.hpp"
#include "../Animation/Skeleton.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include <Utility/Log.hpp>
#include <Engine/Hymn.hpp>

using namespace Animation;

Animation::Skeleton::Skeleton() {
}

void Skeleton::Save(std::string path) {
    // Open the file.
    std::ofstream file(path, std::ios::binary);

    if (!file.is_open()) {
        Log() << "Could not save skeleton file: " << path << "\n";
        return;
    }

    // Save size.
    uint32_t size = skeletonBones.size();
    file.write(reinterpret_cast<char *>(&size), sizeof(uint32_t));

    // Save data.
    for (unsigned int i = 0; i < size; ++i) {
        skeletonBones[i]->Save(&file);
    }

    // Close the file.
    file.close();
}

void Skeleton::Load(std::string name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);
    std::string filePath =  (Hymn().GetPath() + "/" + name + ".asset").c_str();

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        Log() << "Could not open skeleton file: " << filePath << "\n";
        file.close();
        return;
    }

    // Clear bones if anything in them.
    skeletonBones.clear();
    skeletonBones.shrink_to_fit();

    uint32_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    for (unsigned int i = 0; i < size; ++i) {
        SkeletonBone * bone = new SkeletonBone;
        bone->Load(&file);
        skeletonBones.push_back(bone);
    }

    InitSkeleton();

    file.close();
}

void Skeleton::InitSkeleton() {
    // Setup root.
    skeletonBones[0]->parentId = 0;
    skeletonBones[0]->globalTx = skeletonBones[0]->localTx;
    skeletonBones[0]->inversed = glm::inverse(skeletonBones[0]->globalTx);

    // Setup rest of skeleton.
    for (unsigned int i = 1; i < skeletonBones.size(); ++i) {
        skeletonBones[i]->globalTx = skeletonBones[i]->localTx * skeletonBones[skeletonBones[i]->parentId]->globalTx;
        skeletonBones[i]->inversed = glm::inverse(skeletonBones[i]->globalTx);
    }
}
