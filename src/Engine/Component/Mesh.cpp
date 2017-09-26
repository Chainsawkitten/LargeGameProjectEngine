#include "Mesh.hpp"

#include "../Geometry/Model.hpp"
#include "../Hymn.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"

using namespace Component;

Mesh::Mesh(Entity* entity) : SuperComponent(entity) {
    
}

Mesh::~Mesh() {
    Geometry::Model* model = dynamic_cast<Geometry::Model*>(geometry);
    if (model != nullptr)
        Managers().resourceManager->FreeModel(model);
}

Json::Value Mesh::Save() const {
    Json::Value component;
    
    Geometry::Model* model = dynamic_cast<Geometry::Model*>(geometry);
    if (model != nullptr)
        component["model"] = model->name;
    
    return component;
}

void Mesh::Load(const Json::Value& node) {
    std::string meshName = node.get("model", "").asString();
    geometry = Managers().resourceManager->CreateModel(meshName);
}
