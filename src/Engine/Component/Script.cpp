#include "Script.hpp"

#include <angelscript.h>
#include "../Manager/Managers.hpp"
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
}

Json::Value Script::Save() const {
    Json::Value component;
    if (scriptFile != nullptr)
        component["scriptName"] = scriptFile->path + scriptFile->name;
    
    return component;
}
