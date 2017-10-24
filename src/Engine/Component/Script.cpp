#include "Script.hpp"

#include <angelscript.h>
#include "../Manager/Managers.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Entity/Entity.hpp"
#include "../Script/ScriptFile.hpp"

using namespace Component;

Script::Script() {
}

Script::~Script() {
    if (instance != nullptr)
        instance->Release();

    if (scriptFile != nullptr)
        Managers().resourceManager->FreeScriptFile(scriptFile);

    ClearPropertyMap();
}

Json::Value Script::Save() const {
    Json::Value component;
    if (scriptFile != nullptr)
        component["scriptName"] = scriptFile->path + scriptFile->name;

    for (auto& name_pair : propertyMap) {
        const std::string& name = name_pair.first;

        int typeId = name_pair.second.first;
        void* varPointer = name_pair.second.second;
        if (typeId == asTYPEID_INT32) {
            component["propertyMap"][name][std::to_string(typeId)] = *(int*)varPointer;
        } else if (typeId == asTYPEID_FLOAT) {
            component["propertyMap"][name][std::to_string(typeId)] = *(float*)varPointer;
        } else if (typeId == Managers().scriptManager->GetStringDeclarationID()) {
            std::string* str = (std::string*)varPointer;
            component["propertyMap"][name][std::to_string(typeId)] = *str;
        }
    }

    return component;
}

void Script::FillPropertyMap() {
    int propertyCount = instance->GetPropertyCount();

    for (int n = 0; n < propertyCount; n++) {
        int typeId = instance->GetPropertyTypeId(n);
        void* varPointer = instance->GetAddressOfProperty(n);

        auto it = propertyMap.find(instance->GetPropertyName(n));
        if (it != propertyMap.end())
            if (propertyMap[instance->GetPropertyName(n)].first == typeId)
                continue;

        if (typeId == asTYPEID_INT32) {
            int* mapValue = new int();
            *mapValue = *(int*)varPointer;
            propertyMap[instance->GetPropertyName(n)] = std::pair<int, void*>(typeId, mapValue);
        } else if (typeId == asTYPEID_FLOAT) {
            float* mapValue = new float();
            *mapValue = *(float*)varPointer;
            propertyMap[instance->GetPropertyName(n)] = std::pair<int, void*>(typeId, mapValue);
        } else if (typeId == instance->GetEngine()->GetTypeIdByDecl("string")) {
            std::string* str = (std::string*)varPointer;
            if (str) {
                std::string* mapValue = new std::string();
                *mapValue = *(std::string*)varPointer;
                propertyMap[instance->GetPropertyName(n)] = std::pair<int, void*>(typeId, mapValue);
            }
        }
    }
}

/// Clears the property map.
void Script::ClearPropertyMap() {
    for (auto pair : propertyMap) {
        int typeId = pair.second.first;
        void* varPointer = pair.second.second;

        if (typeId == asTYPEID_INT32) {
            delete (int*)varPointer;

        } else if (typeId == asTYPEID_FLOAT) {
            delete (float*)varPointer;
        } else if (typeId == Managers().scriptManager->GetStringDeclarationID()) {
            delete (std::string*)varPointer;
        }
    }

    propertyMap.clear();
}