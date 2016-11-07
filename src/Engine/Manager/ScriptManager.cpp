#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"

void MessageCallback(const asSMessageInfo* message, void* param) {
    Log() << message->section << " (" << message->row << ", " << message->col << " : ";
    
    switch (message->type) {
    case asMSGTYPE_ERROR:
        Log() << "Error";
        break;
    case asMSGTYPE_INFORMATION:
        Log() << "Information";
        break;
    case asMSGTYPE_WARNING:
        Log() << "Warning";
        break;
    }
    
    Log() << " : " << message->message << "\n";
}

ScriptManager::ScriptManager() {
    
}

ScriptManager::~ScriptManager() {
    
}

void ScriptManager::TestScripting() {
    // Create the script engine
    asIScriptEngine* engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL); assert(r >= 0);
    
    // Register add-ons.
    RegisterStdString(engine);
    
    // Clean up.
    engine->ShutDownAndRelease();
}
