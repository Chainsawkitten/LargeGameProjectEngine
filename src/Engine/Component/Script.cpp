#include "Script.hpp"

#include <string>
#include <angelscript.h>
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Manager/ScriptManager.hpp"
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
        component["scriptName"] = scriptFile->name;
    
    return component;
}
