#include "Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../Util/Json.hpp"

using namespace Component;

Transform::Transform(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Transform::Save() const {
    Json::Value component;
    component["position"] = Json::SaveVec3(position);
    component["scale"] = Json::SaveVec3(scale);
    component["rotation"] = Json::SaveVec3(rotation);
    return component;
}

void Transform::Load(const Json::Value& node) {
    position = Json::LoadVec3(node["position"]);
    scale = Json::LoadVec3(node["scale"]);
    rotation = Json::LoadVec3(node["rotation"]);
}

glm::mat4 Transform::GetModelMatrix() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    return glm::translate(glm::mat4(), position) * orientation * glm::scale(glm::mat4(), scale);
}

glm::mat4 Transform::GetOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
}

glm::mat4 Transform::GetCameraOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
}

glm::vec3 Transform::GetDirection() const {
    return glm::normalize(glm::vec3(GetOrientation() * glm::vec4(0.f, 0.f, 1.f, 0.f)));
}
