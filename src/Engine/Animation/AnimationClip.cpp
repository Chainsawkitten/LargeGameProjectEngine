#include "AnimationClip.hpp"
#include <fstream>

void Animation::AnimationClip::Load(const std::string & name) {
    std::ifstream file(name);
    
    animation = new Animation;
    animation->Load(&file);
    file.close();
}

void Animation::AnimationClip::Play(float deltaTime, const Skeleton & skeleton, float playbackSpeed) {
    /// @todo Implement this.
}
