#include "EntityEditor.hpp"

#include <Engine/Component/AnimationController.hpp>
#include <Engine/Component/Animation.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Lens.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/PointLight.hpp>
#include <Engine/Component/SpotLight.hpp>
#include <Engine/Component/Listener.hpp>
#include <Engine/Component/RigidBody.hpp>
#include <Engine/Component/Script.hpp>
#include <Engine/Component/Shape.hpp>
#include <Engine/Component/SoundSource.hpp>
#include <Engine/Component/ParticleEmitter.hpp>
#include <Engine/Animation/AnimationController.hpp>
#include <Engine/Component/VRDevice.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Texture/TextureAsset.hpp>
#include <Video/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/ParticleManager.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Hymn.hpp>
#include <angelscript.h>

#include "../../Util/EditorSettings.hpp"
#include "../FileSelector.hpp"
#include "../../ImGui/GuiHelpers.hpp"
#include "../../Resources.hpp"
#include <imgui_internal.h>
#include "PlaneShapeEditor.hpp"
#include "SphereShapeEditor.hpp"

namespace Physics {
    class Shape;
}

using namespace GUI;

EntityEditor::EntityEditor() {
    name[0] = '\0';
    AddEditor<Component::AnimationController>("Animation controller", std::bind(&EntityEditor::AnimationControllerEditor, this, std::placeholders::_1));
    AddEditor<Component::Mesh>("Mesh", std::bind(&EntityEditor::MeshEditor, this, std::placeholders::_1));
    AddEditor<Component::Lens>("Lens", std::bind(&EntityEditor::LensEditor, this, std::placeholders::_1));
    AddEditor<Component::Material>("Material", std::bind(&EntityEditor::MaterialEditor, this, std::placeholders::_1));
    AddEditor<Component::DirectionalLight>("Directional light", std::bind(&EntityEditor::DirectionalLightEditor, this, std::placeholders::_1));
    AddEditor<Component::PointLight>("Point light", std::bind(&EntityEditor::PointLightEditor, this, std::placeholders::_1));
    AddEditor<Component::SpotLight>("Spot light", std::bind(&EntityEditor::SpotLightEditor, this, std::placeholders::_1));
    AddEditor<Component::Listener>("Listener", std::bind(&EntityEditor::ListenerEditor, this, std::placeholders::_1));
    AddEditor<Component::RigidBody>("Rigid body", std::bind(&EntityEditor::RigidBodyEditor, this, std::placeholders::_1));
    AddEditor<Component::Script>("Script", std::bind(&EntityEditor::ScriptEditor, this, std::placeholders::_1));
    AddEditor<Component::Shape>("Shape", std::bind(&EntityEditor::ShapeEditor, this, std::placeholders::_1));
    AddEditor<Component::SoundSource>("Sound source", std::bind(&EntityEditor::SoundSourceEditor, this, std::placeholders::_1));
    AddEditor<Component::ParticleEmitter>("Particle emitter", std::bind(&EntityEditor::ParticleEmitterEditor, this, std::placeholders::_1));
    AddEditor<Component::VRDevice>("VR device", std::bind(&EntityEditor::VRDeviceEditor, this, std::placeholders::_1));

    shapeEditors.push_back(new SphereShapeEditor());
    shapeEditors.push_back(new PlaneShapeEditor());
    selectedShape = 0;
}

EntityEditor::~EntityEditor() {
    for (auto shapeEditor : shapeEditors) {
        delete shapeEditor;
    }
}

void EntityEditor::Show() {
    if (ImGui::Begin(("Entity: " + entity->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(entity))).c_str(), &visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ShowBorders)) {
        ImGui::InputText("Name", name, 128);
        entity->name = name;
        ImGui::Text("Transform");
        ImGui::ShowHelpMarker("The entity's position, rotation and scale.", 75.f);
        ImGui::Indent();

        if (EditorSettings::GetInstance().GetBool("Grid Snap")) {
            int toNearest = EditorSettings::GetInstance().GetLong("Grid Snap Size");

            int value = entity->position.x;
            int rest = value % toNearest;

            if (rest > (toNearest / 2)) {
                entity->position.x = (value - rest) + toNearest;
            } else {
                entity->position.x = (value - rest);
            }

            value = entity->position.y;
            rest = value % toNearest;

            if (rest > (toNearest / 2)) {
                entity->position.y = (value - rest) + toNearest;
            } else {
                entity->position.y = (value - rest);
            }

            value = entity->position.z;
            rest = value % toNearest;

            if (rest > (toNearest / 2)) {
                entity->position.z = (value - rest) + toNearest;
            } else {
                entity->position.z = (value - rest);
            }
        }

        ImGui::DraggableVec3("Position", entity->position);
        ImGui::DraggableVec3("Rotation", entity->rotation);
        ImGui::DraggableVec3("Scale", entity->scale);
        ImGui::Text("Unique Identifier: %u", entity->GetUniqueIdentifier());
        ImGui::Checkbox("Is entity static", &entity->isStatic);
        ImGui::Unindent();
        if (!entity->IsScene()) {
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
    }

    ImGui::End();
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    strcpy(name, entity->name.c_str());

    auto shapeComp = this->entity->GetComponent<Component::Shape>();
    if (shapeComp) {
        Physics::Shape& shape = *shapeComp->GetShape();
        for (uint32_t i = 0; i < shapeEditors.size(); ++i) {
            if (shapeEditors[i]->SetFromShape(shape)) {
                selectedShape = i;
            }
        }
    }
}

Entity* EntityEditor::GetEntity() {
    return entity;
}

bool EntityEditor::ShowsEntity(Entity* entity) {
    return this->entity == entity;
}

bool EntityEditor::IsVisible() const {
    return visible;
}

void EntityEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void EntityEditor::AnimationControllerEditor(Component::AnimationController* animationController) {
    ImGui::Indent(); 
    if (ImGui::Button("Select animation controller##Animation"))
        ImGui::OpenPopup("Select animation controller##Animation");

    if (ImGui::BeginPopup("Select animation controller##Animation")) {
        ImGui::Text("Animation controllers");
        ImGui::Separator();

        if (resourceSelector.Show(ResourceList::Resource::Type::ANIMATION_CONTROLLER)) {
            if (animationController != nullptr)
                Managers().resourceManager->FreeAnimationController(animationController->controller);

            animationController->controller = Managers().resourceManager->CreateAnimationController(resourceSelector.GetSelectedResource().GetPath());
        }

        ImGui::EndPopup();
    }

    if (ImGui::Button("Select skeleton##Skeleton"))
        ImGui::OpenPopup("Select skeleton##Skeleton");

    if (ImGui::BeginPopup("Select skeleton##Skeleton")) {
        ImGui::Text("Skeletons");
        ImGui::Separator();

        if (resourceSelector.Show(ResourceList::Resource::Type::SKELETON)) {
            if (animationController != nullptr)
                Managers().resourceManager->FreeSkeleton(animationController->skeleton);

            animationController->skeleton = Managers().resourceManager->CreateSkeleton(resourceSelector.GetSelectedResource().GetPath());
        }

        ImGui::EndPopup();
    }

    ImGui::Unindent();
}

void EntityEditor::MeshEditor(Component::Mesh* mesh) {
    ImGui::Indent();
    if (ImGui::Button("Select model##Mesh"))
        ImGui::OpenPopup("Select model##Mesh");
    
    if (ImGui::BeginPopup("Select model##Mesh")) {
        ImGui::Text("Models");
        ImGui::Separator();
        
        if (resourceSelector.Show(ResourceList::Resource::Type::MODEL)) {
            if (mesh->geometry != nullptr)
                Managers().resourceManager->FreeModel(dynamic_cast<Geometry::Model*>(mesh->geometry));
            
            mesh->geometry = Managers().resourceManager->CreateModel(resourceSelector.GetSelectedResource().GetPath());
        }
        
        ImGui::EndPopup();
    }
    ImGui::Unindent();
}

void EntityEditor::LensEditor(Component::Lens* lens) {
    ImGui::Indent();
    ImGui::DraggableFloat("Field of view", lens->fieldOfView, 0.0f, 180.f);
    ImGui::DraggableFloat("Z near", lens->zNear, 0.0f);
    ImGui::DraggableFloat("Z far", lens->zFar, 0.0f);
    ImGui::Unindent();
}

void EntityEditor::MaterialEditor(Component::Material* material) {
    // Albedo
    ImGui::Text("Albedo");
    ImGui::Indent();
    if (material->albedo->GetTexture()->IsLoaded())
        ImGui::Image((void*) material->albedo->GetTexture()->GetTextureID(), ImVec2(128, 128));
    
    if (ImGui::Button("Select albedo texture"))
        albedoShow = true;

    if (albedoShow) {
        ImGui::Begin("Textures", &albedoShow, ImGuiWindowFlags_ShowBorders);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->albedo != Hymn().defaultAlbedo)
                Managers().resourceManager->FreeTextureAsset(material->albedo);

            material->albedo = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }
    ImGui::Unindent();
    
    // Normal
    ImGui::Text("Normal");
    ImGui::Indent();
    if (material->normal->GetTexture()->IsLoaded())
        ImGui::Image((void*) material->normal->GetTexture()->GetTextureID(), ImVec2(128, 128));
    
    if (ImGui::Button("Select normal texture"))
        normalShow = true;
    
    if (normalShow) {
        ImGui::Begin("Textures", &normalShow, ImGuiWindowFlags_ShowBorders);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->normal != Hymn().defaultNormal)
                Managers().resourceManager->FreeTextureAsset(material->normal);
            
            material->normal = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }
    ImGui::Unindent();

    // Metallic
    ImGui::Text("Metallic");
    ImGui::Indent();
    if (material->metallic->GetTexture()->IsLoaded())
        ImGui::Image((void*) material->metallic->GetTexture()->GetTextureID(), ImVec2(128, 128));
    
    if (ImGui::Button("Select metallic texture"))
        metallicShow = true;

    if (metallicShow) {
        ImGui::Begin("Textures", &metallicShow, ImGuiWindowFlags_ShowBorders);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->metallic != Hymn().defaultMetallic)
                Managers().resourceManager->FreeTextureAsset(material->metallic);
            
            material->metallic = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }
    ImGui::Unindent();

    // Roughness
    ImGui::Text("Roughness");
    ImGui::Indent();
    if (material->roughness->GetTexture()->IsLoaded())
        ImGui::Image((void*) material->roughness->GetTexture()->GetTextureID(), ImVec2(128, 128));
    
    if (ImGui::Button("Select roughness texture"))
        roughnessShow = true;
    
    if (roughnessShow) {
        ImGui::Begin("Textures", &roughnessShow, ImGuiWindowFlags_ShowBorders);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->roughness != Hymn().defaultRoughness)
                Managers().resourceManager->FreeTextureAsset(material->roughness);
            
            material->roughness = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }
    ImGui::Unindent();
}

void EntityEditor::DirectionalLightEditor(Component::DirectionalLight* directionalLight) {
    ImGui::Indent();
    ImGui::ColorEdit3("Color", &directionalLight->color[0]);
    ImGui::DraggableFloat("Ambient coefficient", directionalLight->ambientCoefficient, 0.0f);
    ImGui::Unindent();
}

void EntityEditor::PointLightEditor(Component::PointLight* pointLight) {
    ImGui::Indent();
    ImGui::ColorEdit3("Color", &pointLight->color[0]);
    ImGui::DraggableFloat("Attenuation", pointLight->attenuation, 0.0f);
    ImGui::DraggableFloat("Intensity", pointLight->intensity, 0.0f);
    ImGui::Unindent();
}

void EntityEditor::SpotLightEditor(Component::SpotLight* spotLight) {
    ImGui::Indent();
    ImGui::ColorEdit3("Color", &spotLight->color[0]);
    ImGui::DraggableFloat("Ambient coefficient", spotLight->ambientCoefficient, 0.0f);
    ImGui::DraggableFloat("Attenuation", spotLight->attenuation, 0.0f);
    ImGui::DraggableFloat("Intensity", spotLight->intensity, 0.0f);
    ImGui::DraggableFloat("Cone angle", spotLight->coneAngle, 0.0f, 180.f);
    ImGui::Unindent();
}

void EntityEditor::ListenerEditor(Component::Listener* listener) {
    
}

void EntityEditor::RigidBodyEditor(Component::RigidBody* rigidBody) {
    auto shapeComp = rigidBody->entity->GetComponent<Component::Shape>();
    if (shapeComp) {
        ImGui::Indent();
        if (ImGui::InputFloat("Mass", &rigidBodyMass))
            Managers().physicsManager->SetMass(rigidBody, rigidBodyMass);
        ImGui::Unindent();
    } else {
        ImGui::Indent();
        ImGui::TextWrapped("A rigid body is only valid with a complementary shape component. Please add one to allow editing this component.");
        ImGui::Unindent();
    }
}

void EntityEditor::ScriptEditor(Component::Script* script) {

    ImGui::Indent();

    if (ImGui::Button("Select script"))
        ImGui::OpenPopup("Select script");

    if (ImGui::BeginPopup("Select script")) {
        ImGui::Text("Scripts");
        ImGui::Separator();

        for (ScriptFile* scriptFile : Hymn().scripts) {
            if (ImGui::Selectable(scriptFile->name.c_str()))
                script->scriptFile = scriptFile;
        }

        ImGui::EndPopup();
    }

    if (script->scriptFile != nullptr) {
        ImGui::Text(script->scriptFile->name.c_str());
        ImGui::Separator();

        if (ImGui::Button("Fetch properties")) {

            Managers().scriptManager->FillPropertyMap(script);
                
        }

        if (script->instance != nullptr) {

            int propertyCount = script->instance->GetPropertyCount();

            for (int n = 0; n < propertyCount; n++) {

                int typeId = script->instance->GetPropertyTypeId(n);
                void *varPointer = script->instance->GetAddressOfProperty(n);
                if (typeId == asTYPEID_INT32){
                    ImGui::InputInt(script->instance->GetPropertyName(n), (int*)script->propertyMap[script->instance->GetPropertyName(n)].second, 0.0f);
                }
                else if (typeId == asTYPEID_FLOAT){
                    ImGui::DraggableFloat(script->instance->GetPropertyName(n), *(float*)script->propertyMap[script->instance->GetPropertyName(n)].second, 0.0f);
                }
                /// @todo This will be used to handle objects in the scripts
                //else if (typeId & asTYPEID_SCRIPTOBJECT){
                //    asIScriptObject *obj = (asIScriptObject*)varPointer;
                //}
                else if (typeId == script->instance->GetEngine()->GetTypeIdByDecl("string")){
                    
                    std::map<std::string, std::pair<int, void*>>::iterator it = script->propertyMap.find(script->instance->GetPropertyName(n));
                    if (it != script->propertyMap.end()) {

                        std::string *str = (std::string*)script->propertyMap[script->instance->GetPropertyName(n)].second;

                        //We have to put a limit to the size of the string because we want to use a buffer so we don't have to reallocate it every frame.
                        //I decided to use 128 because that's what we use for the name of the script.
                        if (str->size() <= 128) {

                            std::copy(str->begin(), str->end(), stringPropertyBuffer);
                            stringPropertyBuffer[str->size()] = '\0';

                            ImGui::InputText(script->instance->GetPropertyName(n), stringPropertyBuffer, 128);

                            *str = stringPropertyBuffer;

                        }
                        else
                            ImGui::Text("%s = <TOO BIG>\n", script->instance->GetPropertyName(n));

                    }
                    else
                        ImGui::Text("%s = <null>\n", script->instance->GetPropertyName(n));
                }

            }

            if (ImGui::Button("Reset properties")) {

                script->ClearPropertyMap();
                Managers().scriptManager->FillPropertyMap(script);

            }

        }
        ImGui::Separator();

        ImGui::Text("Entity References");
        // Display current entity references
        for (size_t i = 0; i != script->refList.size(); ++i) {
            ImGui::Text(script->refList[i]->name.c_str());
            ImGui::SameLine(ImGui::GetWindowWidth() - 30);
            if(ImGui::SmallButton(("x###remove" + std::to_string(i)).c_str())){
                script->refList.erase(script->refList.begin() + i);
                break;
            }
        }

        // Choosing other entity references
        if (ImGui::Button("Add entity reference"))
            ImGui::OpenPopup("Add entity reference");

        if (ImGui::BeginPopup("Add entity reference")) {
            ImGui::Text("Entities");
            ImGui::Separator();
            for (Entity* entity : Hymn().world.GetEntities()) /// @todo Change into a prettier tree structure or something, later.
                if (ImGui::Selectable(entity->name.c_str()))
                    script->refList.push_back(entity);

            ImGui::EndPopup();
        }

    }
    else
        ImGui::Text("No script loaded");


    ImGui::Unindent();

}

void EntityEditor::ShapeEditor(Component::Shape* shape) {
    if (ImGui::Combo("Shape", &selectedShape, [](void* data, int idx, const char** outText) -> bool {
        IShapeEditor* editor = *(reinterpret_cast<IShapeEditor**>(data) + idx);
        *outText = editor->Label();
        return true;
    }, shapeEditors.data(), shapeEditors.size())) {
        shapeEditors[selectedShape]->Apply(shape);
    }

    if (selectedShape != -1) {
        shapeEditors[selectedShape]->Show(shape);
    }
}

void EntityEditor::SoundSourceEditor(Component::SoundSource* soundSource) {
    ImGui::Text("Sound");
    ImGui::Indent();
    if (ImGui::Button("Select sound"))
        ImGui::OpenPopup("Select sound");
    
    if (ImGui::BeginPopup("Select sound")) {
        ImGui::Text("Sounds");
        ImGui::Separator();
        
        if (resourceSelector.Show(ResourceList::Resource::Type::SOUND)) {
            if (soundSource->soundBuffer != nullptr)
                Managers().resourceManager->FreeSound(soundSource->soundBuffer);
            
            soundSource->soundBuffer = Managers().resourceManager->CreateSound(resourceSelector.GetSelectedResource().GetPath());
        }
        
        ImGui::EndPopup();
    }
    ImGui::Unindent();
    ImGui::Text("Sound properties");
    ImGui::Indent();
    ImGui::DraggableFloat("Volume", soundSource->volume, 0.0f, 1.0f);
    ImGui::Checkbox("Loop", &soundSource->loop);
    ImGui::Unindent();
}

void EntityEditor::ParticleEmitterEditor(Component::ParticleEmitter* particleEmitter) {
    ImGui::Text("Particle");
    ImGui::Indent();
    int rows = Managers().particleManager->GetTextureAtlasRows();
    float column = static_cast<float>(particleEmitter->particleType.textureIndex % rows);
    float row = static_cast<float>(particleEmitter->particleType.textureIndex / rows);
    ImGui::Image((void*) Managers().particleManager->GetTextureAtlas()->GetTextureID(), ImVec2(128, 128), ImVec2(column / rows, row / rows), ImVec2((column + 1.f) / rows, (row + 1.f) / rows));
    ImGui::InputInt("Texture index", &particleEmitter->particleType.textureIndex);
    ImGui::ColorEdit3("Color", &particleEmitter->particleType.color[0]);
    ImGui::DraggableVec3("Min velocity", particleEmitter->particleType.minVelocity);
    ImGui::DraggableVec3("Max velocity", particleEmitter->particleType.maxVelocity);
    ImGui::DraggableFloat("Average lifetime", particleEmitter->particleType.averageLifetime, 0.0f);
    ImGui::DraggableFloat("Lifetime variance", particleEmitter->particleType.lifetimeVariance, 0.0f);
    ImGui::DraggableVec2("Average size", particleEmitter->particleType.averageSize, 0.0f);
    ImGui::DraggableVec2("Size variance", particleEmitter->particleType.sizeVariance, 0.0f);
    ImGui::Checkbox("Uniform scaling", &particleEmitter->particleType.uniformScaling);
    ImGui::DraggableFloat("Start alpha", particleEmitter->particleType.startAlpha, 0.0f, 1.0f);
    ImGui::DraggableFloat("Mid alpha", particleEmitter->particleType.midAlpha, 0.0f, 1.0f);
    ImGui::DraggableFloat("End alpha", particleEmitter->particleType.endAlpha, 0.0f, 1.0f);
    ImGui::Unindent();
    
    ImGui::Text("Emitter");
    ImGui::Indent();
    ImGui::DraggableFloat("Average emit time", particleEmitter->averageEmitTime, 0.001f);
    ImGui::DraggableFloat("Emit time variance", particleEmitter->emitTimeVariance, 0.0f);
    
    const char* items[] = { "Point", "Cuboid" };
    int item = static_cast<int>(particleEmitter->emitterType);
    if (ImGui::Combo("Emitter type", &item, items, 2))
        particleEmitter->emitterType = static_cast<Component::ParticleEmitter::EmitterType>(item);
    
    if (particleEmitter->emitterType == Component::ParticleEmitter::CUBOID)
        ImGui::DraggableVec3("Size", particleEmitter->size);
    
    ImGui::Unindent();
    
    ImGui::Text("Preview");
    ImGui::Indent();
    ImGui::Checkbox("Simulate", &particleEmitter->preview);
    ImGui::Unindent();
}

void EntityEditor::VRDeviceEditor(Component::VRDevice* vrDevice) {
    const char* items[] = { "Controller", "Headset" };
    int item = static_cast<int>(vrDevice->type);
    if (ImGui::Combo("Type", &item, items, 2))
        vrDevice->type = static_cast<Component::VRDevice::Type>(item);
    
    if (vrDevice->type == Component::VRDevice::CONTROLLER) {
        ImGui::Text("Controller");
        ImGui::Indent();
        ImGui::InputInt("Controller ID (1 = left, 2 = right)", &vrDevice->controllerID);
        ImGui::Unindent();
    }
}
