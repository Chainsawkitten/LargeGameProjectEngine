#include "EntityEditor.hpp"

#include <Engine/Component/Transform.hpp>
#include <Engine/Component/Physics.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Lens.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/PointLight.hpp>
#include <Engine/Component/SpotLight.hpp>
#include <Engine/Component/Listener.hpp>
#include <Engine/Component/SoundSource.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Geometry/OBJModel.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>

using namespace GUI;

EntityEditor::EntityEditor() {
    AddEditor<Component::Transform>("Transform", std::bind(&TransformEditor, this, std::placeholders::_1));
    AddEditor<Component::Physics>("Physics", std::bind(&PhysicsEditor, this, std::placeholders::_1));
    AddEditor<Component::Mesh>("Mesh", std::bind(&MeshEditor, this, std::placeholders::_1));
    AddEditor<Component::Lens>("Lens", std::bind(&LensEditor, this, std::placeholders::_1));
    AddEditor<Component::Material>("Material", std::bind(&MaterialEditor, this, std::placeholders::_1));
    AddEditor<Component::DirectionalLight>("Directional light", std::bind(&DirectionalLightEditor, this, std::placeholders::_1));
    AddEditor<Component::PointLight>("Point light", std::bind(&PointLightEditor, this, std::placeholders::_1));
    AddEditor<Component::SpotLight>("Spot light", std::bind(&SpotLightEditor, this, std::placeholders::_1));
    AddEditor<Component::Listener>("Listener", std::bind(&ListenerEditor, this, std::placeholders::_1));
    AddEditor<Component::SoundSource>("Sound source", std::bind(&SoundSourceEditor, this, std::placeholders::_1));
}

EntityEditor::~EntityEditor() {
    
}

void EntityEditor::Show() {
    if (ImGui::Begin(("Entity: " + entity->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(entity))).c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        entity->name = name;
        
        if (ImGui::Button("Add component"))
            ImGui::OpenPopup("Add component");
        
        if (ImGui::BeginPopup("Add component")) {
            ImGui::Text("Components");
            ImGui::Separator();
            
            for (Editor& editor : editors) {
                editor.addFunction();
            }
            
            ImGui::EndPopup();
        }
        
        for (Editor& editor : editors) {
            editor.editFunction();
        }
    }
    ImGui::End();
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    strcpy(name, entity->name.c_str());
}

bool EntityEditor::IsVisible() const {
    return visible;
}

void EntityEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void EntityEditor::TransformEditor(Component::Transform* transform) {
    ImGui::InputFloat3("Position", &transform->position[0]);
    ImGui::InputFloat3("Rotation", &transform->rotation[0]);
    ImGui::InputFloat3("Scale", &transform->scale[0]);
}

void EntityEditor::PhysicsEditor(Component::Physics* physics) {
    ImGui::InputFloat3("Velocity", &physics->velocity[0]);
    ImGui::InputFloat("Max velocity", &physics->maxVelocity);
    ImGui::InputFloat3("Angular velocity", &physics->angularVelocity[0]);
    ImGui::InputFloat("Max angular velocity", &physics->maxAngularVelocity);
    ImGui::InputFloat3("Acceleration", &physics->acceleration[0]);
    ImGui::InputFloat3("Angular acceleration", &physics->angularAcceleration[0]);
    ImGui::InputFloat("Velocity drag factor", &physics->velocityDragFactor);
    ImGui::InputFloat("Angular drag factor", &physics->angularDragFactor);
    ImGui::InputFloat("Gravity factor", &physics->gravityFactor);
    ImGui::InputFloat3("Moment of inertia", &physics->momentOfInertia[0]);
}

void EntityEditor::MeshEditor(Component::Mesh* mesh) {
    if (ImGui::Button("Select model"))
        ImGui::OpenPopup("Select model");
    
    if (ImGui::BeginPopup("Select model")) {
        ImGui::Text("Models");
        ImGui::Separator();
        
        for (Geometry::OBJModel* model : Hymn().models) {
            if (ImGui::Selectable(model->name.c_str()))
                mesh->geometry = model;
        }
        
        ImGui::EndPopup();
    }
}

void EntityEditor::LensEditor(Component::Lens* lens) {
    ImGui::InputFloat("Field of view", &lens->fieldOfView);
    ImGui::InputFloat("Z near", &lens->zNear);
    ImGui::InputFloat("Z far", &lens->zFar);
}

void EntityEditor::MaterialEditor(Component::Material* material) {
    // Diffuse
    if (ImGui::Button("Select diffuse texture"))
        ImGui::OpenPopup("Select diffuse texture");
    
    if (ImGui::BeginPopup("Select diffuse texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->diffuse = texture;
        }
        
        ImGui::EndPopup();
    }
    
    // Normal
    if (ImGui::Button("Select normal texture"))
        ImGui::OpenPopup("Select normal texture");
    
    if (ImGui::BeginPopup("Select normal texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->normal = texture;
        }
        
        ImGui::EndPopup();
    }
    
    // Specular
    if (ImGui::Button("Select specular texture"))
        ImGui::OpenPopup("Select specular texture");
    
    if (ImGui::BeginPopup("Select specular texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->specular = texture;
        }
        
        ImGui::EndPopup();
    }
    
    // Glow
    if (ImGui::Button("Select glow texture"))
        ImGui::OpenPopup("Select glow texture");
    
    if (ImGui::BeginPopup("Select glow texture")) {
        ImGui::Text("Textures");
        ImGui::Separator();
        
        for (Texture2D* texture : Hymn().textures) {
            if (ImGui::Selectable(texture->name.c_str()))
                material->glow = texture;
        }
        
        ImGui::EndPopup();
    }
}

void EntityEditor::DirectionalLightEditor(Component::DirectionalLight* directionalLight) {
    ImGui::InputFloat3("Color", &directionalLight->color[0]);
    ImGui::InputFloat("Ambient coefficient", &directionalLight->ambientCoefficient);
}

void EntityEditor::PointLightEditor(Component::PointLight* pointLight) {
    ImGui::InputFloat3("Color", &pointLight->color[0]);
    ImGui::InputFloat("Ambient coefficient", &pointLight->ambientCoefficient);
    ImGui::InputFloat("Attenuation", &pointLight->attenuation);
    ImGui::InputFloat("Intensity", &pointLight->intensity);
}

void EntityEditor::SpotLightEditor(Component::SpotLight* spotLight) {
    ImGui::InputFloat3("Color", &spotLight->color[0]);
    ImGui::InputFloat("Ambient coefficient", &spotLight->ambientCoefficient);
    ImGui::InputFloat("Attenuation", &spotLight->attenuation);
    ImGui::InputFloat("Intensity", &spotLight->intensity);
    ImGui::InputFloat("Cone angle", &spotLight->coneAngle);
}

void EntityEditor::ListenerEditor(Component::Listener* listener) {
    
}

void EntityEditor::SoundSourceEditor(Component::SoundSource* soundSource) {
    if (ImGui::Button("Select sound"))
        ImGui::OpenPopup("Select sound");
    
    if (ImGui::BeginPopup("Select sound")) {
        ImGui::Text("Sounds");
        ImGui::Separator();
        
        for (Audio::SoundBuffer* sound : Hymn().sounds) {
            if (ImGui::Selectable(sound->name.c_str()))
                soundSource->soundBuffer = sound;
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::InputFloat("Pitch", &soundSource->pitch);
    ImGui::InputFloat("Gain", &soundSource->gain);
    ImGui::Checkbox("Loop", &soundSource->loop);
}
