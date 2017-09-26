#include "Animation.hpp"

#include "../Hymn.hpp"
#include "../Geometry/Model.hpp"

Component::Animation::Animation(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Component::Animation::Save() const {
    Json::Value component;
    if (riggedModel != nullptr)
        component["riggedModel"] = riggedModel->name;
    else
        component["riggedModel"] = "null";
    return component;
}

void Component::Animation::Load(const Json::Value& node) {
    std::string name = node.get("riggedModel", "").asString();
    /// @todo Fix animation.
    /*for (Geometry::Model* model : Hymn().models) {
        if (model->name == name)
            riggedModel = model;
    }*/
}
