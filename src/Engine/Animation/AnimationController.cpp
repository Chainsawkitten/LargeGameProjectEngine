#include "AnimationController.hpp"
#include "Skeleton.hpp"
#include "AnimationClip.hpp"
#include <Utility/Log.hpp>
#include "../Hymn.hpp"

using namespace Animation;

AnimationController::~AnimationController() {
    Clear();
}

void AnimationController::Save(const std::string & path) {
    // Open file.
    std::ofstream file(path, std::ios::binary);

    // Check if file is open, if not log and early return.
    if (!file.is_open()) {
        Log() << "Could not save animation controller file: " << path << "\n";
        file.close();
        return;
    }

    uint32_t numNodes = animationNodes.size();
    file.write(reinterpret_cast<char*>(&numNodes), sizeof(uint32_t));

    for (Node* node : animationNodes) {
        if (typeid(*node) == typeid(AnimationAction)) {
            NodeType nodetype = ACTION;
            file.write(reinterpret_cast<char*>(&nodetype), sizeof(NodeType));
        } else {
            NodeType nodetype = TRANSITION;
            file.write(reinterpret_cast<char*>(&nodetype), sizeof(NodeType));
        }

        node->Save(&file);
    }

    // Close file.
    file.close();
}

void AnimationController::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);

    // Open file.
    std::ifstream file(Hymn().GetPath() + "/" + name + ".asset", std::ios::binary);

    // Check if file is open, if not log and early return.
    if (!file.is_open()) {
        Log() << "Could not load animation controller file: " << Hymn().GetPath() + "/" + name + ".asset" << "\n";
        file.close();
        return;
    }

    uint32_t numNodes = 0;
    file.read(reinterpret_cast<char*>(&numNodes), sizeof(uint32_t));
    for (unsigned int i = 0; i < numNodes; ++i)  {
        NodeType nodetype;
        file.read(reinterpret_cast<char*>(&nodetype), sizeof(NodeType));
        if (nodetype == ACTION) {
            AnimationAction * node = new AnimationAction;
            node->Load(&file);
            animationNodes.push_back(node);
        } else {
            AnimationTransition * node = new AnimationTransition;
            node->Load(&file);
            animationNodes.push_back(node);
        }
    }

    // Close file.
    file.close();
}

void Animation::AnimationController::Clear() {
    for (Node * node : animationNodes) {
        delete node;
    }

    animationNodes.clear();
    animationNodes.shrink_to_fit();
}

