#pragma once

#include <map>
#include <typeinfo>
#include "../Scene/Scene.hpp"
#include <json/json.h>

namespace Component {
    class SuperComponent;
}

/// %Entity containing various components.
class Entity {
    public:
        /// Create new entity.
        /**
         * @param scene The scene in which the entity is contained.
         */
        Entity(Scene* scene, const std::string& name = "");
        
        /// Destructor.
        ~Entity();
        
        /// Adds component with type T.
        /**
         * @return The created component.
         */
        template<typename T> T* AddComponent();
        
        /// Gets component with type T.
        /**
         * @return The requested component (or nullptr).
         */
        template<typename T> T* GetComponent();
        
        /// Save the entity.
        /**
         * @return JSON value to be stored on disk.
         */
        Json::Value Save() const;
        
        /// Load entity from JSON node.
        /**
         * @param node JSON node to load from.
         */
        void Load(const Json::Value& node);
        
        /// Name of the entity.
        std::string name;
        
    private:
        Scene* scene;
        
        std::map<const std::type_info*, Component::SuperComponent*> components;
};

template<typename T> T* Entity::AddComponent() {
    const std::type_info* componentType = &typeid(T*);
    if (components.find(componentType) != components.end())
        return nullptr;
    T* component = new T(this);
    components[componentType] = component;
    scene->AddComponent(component, componentType);
    return component;
}

template<typename T> T* Entity::GetComponent() {
    if (components.count(&typeid(T*)) != 0) {
        return static_cast<T*>(components[&typeid(T*)]);
    } else {
        return nullptr;
    }
}
