#include "Entity.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../Component/Animation.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/RigidBody.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Script.hpp"
#include "../Component/Shape.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Util/Json.hpp"
#include "../Util/FileSystem.hpp"
#include <Utility/Log.hpp>
#include "../Hymn.hpp"
#include <fstream>
#include "../Manager/Managers.hpp"
#include "../Manager/ParticleManager.hpp"
#include "../Manager/PhysicsManager.hpp"
#include "../Manager/RenderManager.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Manager/SoundManager.hpp"

Entity::Entity(World* world, const std::string& name) : name ( name ) {
    this->world = world;
}

Entity::~Entity() {
    
}

Entity* Entity::GetParent() const {
    return parent;
}

Entity* Entity::AddChild(const std::string& name) {
    Entity* child = world->CreateEntity(name);
    child->parent = this;
    children.push_back(child);
    return child;
}

bool Entity::SetParent(Entity* newParent) {
    //We make sure we're not trying to put the root as a child.
    if (parent != nullptr) {
        //We make sure we're not trying to set a parent as a child to one of it's own children.
        if (!HasChild(newParent)) {
            parent->RemoveChild(this);
            parent = newParent;
            newParent->children.push_back(this);
            
            return true;
        }
    }
    
    return false;
}

bool Entity::HasChild(const Entity* check_child, bool deep) const {
    for (Entity* child : children) {
        if (child->name == check_child->name)
            return true;
        else if (deep)
            child->HasChild(check_child);
    }

    return false;
}

Entity* Entity::InstantiateScene(const std::string& name, const std::string& originScene) {
    Json::Value root;
    Entity* child = AddChild();
    bool error = false;
    std::string filename = Hymn().GetPath() + "/" + name + ".json";

    // Checks if file exists.
    if (FileSystem::FileExists(filename.c_str())) {
        std::ifstream file1(filename);
        file1 >> root;

        CheckIfSceneExists(filename, error, originScene, root);

        if (error == false) {
            std::ifstream file(filename);
            file >> root;
            file.close();
            child->Load(root);
            child->scene = true;
            child->sceneName = name;
        }
    } else {
        child->name = "Error loading scene";
        Log() << "Couldn't find scene to load.";
    }

    if (error) {
        child->name = "Error loading scene";
        Log() << "Scene is added in continous loop.";
    }

    return child;
}

void Entity::CheckIfSceneExists(const std::string& filename, bool& error, const std::string& originScene, Json::Value root) {
    // Loops through all the scene names.
    for (unsigned int i = 0; i < root["children"].size(); ++i) {
        if (root["children"][i]["scene"].asBool()) {
            printf("%s", root["children"][i]["sceneName"].asString().c_str());

            if (originScene == root["children"][i]["sceneName"].asString())
                error = true;
                
            if (error)
                break;
        }

        if (!error)
            CheckIfSceneExists(filename, error, originScene, root["children"][i]);
    }
}

const std::vector<Entity*>& Entity::GetChildren() const {
    return children;
}

Entity* Entity::GetChild(const std::string& name) const {
    for (Entity* child : children) {
        if (child->name == name)
            return child;
    }
    
    return nullptr;
}

bool Entity::RemoveChild(Entity* child) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == child) {
            children.erase(it);
            return true;
        }
    }
    
    return false;
}

bool Entity::IsScene() const {
    return scene;
}

void Entity::Kill() {
    KillHelper();
    
    // Remove this entity from the parent's list of children.
    if (parent != nullptr)
        parent->RemoveChild(this);
}

bool Entity::IsKilled() const {
    return killed;
}

Json::Value Entity::Save() const {
    Json::Value entity;
    entity["name"] = name;
    entity["position"] = Json::SaveVec3(position);
    entity["scale"] = Json::SaveVec3(scale);
    entity["rotation"] = Json::SaveVec3(rotation);
    entity["scene"] = scene;
    entity["uid"] = uniqueIdentifier;
    entity["static"] = isStatic;

    if (scene) {
        entity["sceneName"] = sceneName;
    } else {
        // Save components.
        Save<Component::Animation>(entity, "Animation");
        Save<Component::Lens>(entity, "Lens");
        Save<Component::Mesh>(entity, "Mesh");
        Save<Component::Material>(entity, "Material");
        Save<Component::DirectionalLight>(entity, "DirectionalLight");
        Save<Component::PointLight>(entity, "PointLight");
        Save<Component::SpotLight>(entity, "SpotLight");
        Save<Component::RigidBody>(entity, "RigidBody");
        Save<Component::Listener>(entity, "Listener");
        Save<Component::Script>(entity, "Script");
        Save<Component::Shape>(entity, "Shape");
        Save<Component::SoundSource>(entity, "SoundSource");
        Save<Component::ParticleEmitter>(entity, "ParticleEmitter");
        
        // Save children.
        Json::Value childNodes;
        for (Entity* child : children)
            childNodes.append(child->Save());
        entity["children"] = childNodes;
    }
    
    return entity;
}

void Entity::Load(const Json::Value& node) {
    scene = node["scene"].asBool();
    
    if (scene) {
        sceneName = node["sceneName"].asString();
        
        // Load scene.
        std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scenes" + FileSystem::DELIMITER + sceneName + ".json";
        Json::Value root;
        std::ifstream file(filename);
        file >> root;
        file.close();
        Load(root);
        
        scene = true;
    } else {
        // Load components.
        Load<Component::Animation>(node, "Animation");
        Load<Component::Lens>(node, "Lens");
        Load<Component::Mesh>(node, "Mesh");
        Load<Component::Material>(node, "Material");
        Load<Component::DirectionalLight>(node, "DirectionalLight");
        Load<Component::PointLight>(node, "PointLight");
        Load<Component::SpotLight>(node, "SpotLight");
        Load<Component::RigidBody>(node, "RigidBody");
        Load<Component::Listener>(node, "Listener");
        Load<Component::Script>(node, "Script");
        Load<Component::Shape>(node, "Shape");
        Load<Component::SoundSource>(node, "SoundSource");
        Load<Component::ParticleEmitter>(node, "ParticleEmitter");
        
        // Load children.
        for (unsigned int i=0; i < node["children"].size(); ++i) {
            Entity* entity = AddChild("");
            entity->Load(node["children"][i]);
        }
    }
    
    name = node.get("name", "").asString();
    position = Json::LoadVec3(node["position"]);
    scale = Json::LoadVec3(node["scale"]);
    rotation = Json::LoadVec3(node["rotation"]);
    uniqueIdentifier = node.get("uid", 0).asUInt();
    isStatic = node["static"].asBool();
    
}

glm::mat4 Entity::GetModelMatrix() const {
    glm::mat4 matrix = GetLocalMatrix();
    
    if (parent != nullptr)
        matrix = parent->GetModelMatrix() * matrix;
    
    return matrix;
}

glm::mat4 Entity::GetLocalMatrix() const {
    glm::mat4 matrix = glm::translate(glm::mat4(), position) * GetOrientation() * glm::scale(glm::mat4(), scale);
    return matrix;
}

glm::mat4 Entity::GetOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
}

glm::mat4 Entity::GetCameraOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    orientation = glm::rotate(orientation, glm::radians(rotation.y), glm::vec3(1.f, 0.f, 0.f));
    return glm::rotate(orientation, glm::radians(rotation.x), glm::vec3(0.f, 1.f, 0.f));
}

glm::vec3 Entity::GetDirection() const {
    return glm::normalize(glm::vec3(GetOrientation() * glm::vec4(0.f, 0.f, -1.f, 0.f)));
}

glm::vec3 Entity::GetWorldPosition() const {
    if (parent != nullptr)
        return glm::vec3(parent->GetModelMatrix() * glm::vec4(position, 1.f));
    
    return position;
}

unsigned int Entity::GetUniqueIdentifier() const {
    return uniqueIdentifier;
}

void Entity::SetUniqueIdentifier(unsigned int UID) {
    uniqueIdentifier = UID;
}

Component::SuperComponent* Entity::AddComponent(const std::type_info* componentType) {
    Component::SuperComponent* component;
    
    // Create a component in the correct manager.
    if (*componentType == typeid(Component::Animation*))
        component = Managers().renderManager->CreateAnimation();
    else if (*componentType == typeid(Component::DirectionalLight*))
        component = Managers().renderManager->CreateDirectionalLight();
    else if (*componentType == typeid(Component::Lens*))
        component = Managers().renderManager->CreateLens();
    else if (*componentType == typeid(Component::Listener*))
        component = Managers().soundManager->CreateListener();
    else if (*componentType == typeid(Component::Material*))
        component = Managers().renderManager->CreateMaterial();
    else if (*componentType == typeid(Component::Mesh*))
        component = Managers().renderManager->CreateMesh();
    else if (*componentType == typeid(Component::ParticleEmitter*))
        component = Managers().particleManager->CreateParticleEmitter();
    else if (*componentType == typeid(Component::PointLight*))
        component = Managers().renderManager->CreatePointLight();
    else if (*componentType == typeid(Component::RigidBody*))
        component = Managers().physicsManager->CreateRigidBody(this);
    else if (*componentType == typeid(Component::Script*))
        component = Managers().scriptManager->CreateScript();
    else if (*componentType == typeid(Component::Shape*))
        component = Managers().physicsManager->CreateShape(this);
    else if (*componentType == typeid(Component::SoundSource*))
        component = Managers().soundManager->CreateSoundSource();
    else if (*componentType == typeid(Component::SpotLight*))
        component = Managers().renderManager->CreateSpotLight();
    else {
        Log() << componentType->name() << " not assigned to a manager!" << "\n";
        return nullptr;
    }
    
    // Add component to our map.
    components[componentType] = component;
    
    // Set ourselves as the owner.
    component->entity = this;
    
    return component;
}

void Entity::LoadComponent(const std::type_info* componentType, const Json::Value& node) {
    Component::SuperComponent* component;
    
    // Create a component in the correct manager.
    if (*componentType == typeid(Component::Animation*))
        component = Managers().renderManager->CreateAnimation(node);
    else if (*componentType == typeid(Component::DirectionalLight*))
        component = Managers().renderManager->CreateDirectionalLight(node);
    else if (*componentType == typeid(Component::Lens*))
        component = Managers().renderManager->CreateLens(node);
    else if (*componentType == typeid(Component::Listener*))
        component = Managers().soundManager->CreateListener(node);
    else if (*componentType == typeid(Component::Material*))
        component = Managers().renderManager->CreateMaterial(node);
    else if (*componentType == typeid(Component::Mesh*))
        component = Managers().renderManager->CreateMesh(node);
    else if (*componentType == typeid(Component::ParticleEmitter*))
        component = Managers().particleManager->CreateParticleEmitter(node);
    else if (*componentType == typeid(Component::PointLight*))
        component = Managers().renderManager->CreatePointLight(node);
    else if (*componentType == typeid(Component::RigidBody*))
        component = Managers().physicsManager->CreateRigidBody(this, node);
    else if (*componentType == typeid(Component::Script*))
        component = Managers().scriptManager->CreateScript(node);
    else if (*componentType == typeid(Component::Shape*))
        component = Managers().physicsManager->CreateShape(this, node);
    else if (*componentType == typeid(Component::SoundSource*))
        component = Managers().soundManager->CreateSoundSource(node);
    else if (*componentType == typeid(Component::SpotLight*))
        component = Managers().renderManager->CreateSpotLight(node);
    else {
        Log() << componentType->name() << " not assigned to a manager!" << "\n";
        return;
    }
    
    // Add component to our map.
    components[componentType] = component;
    
    // Set ourselves as the owner.
    component->entity = this;
}

void Entity::KillHelper() {
    killed = true;
    
    for (auto& it : components)
        it.second->Kill();
    
    for (Entity* child : children) {
        child->KillHelper();
    }
}
