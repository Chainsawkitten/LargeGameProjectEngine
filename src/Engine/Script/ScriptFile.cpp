#include "ScriptFile.hpp"

Json::Value ScriptFile::Save() const {
    Json::Value script;
    script["name"] = name;
    
    return script;
}

void ScriptFile::Load(const std::string& name) {
    this->name = name;
}
