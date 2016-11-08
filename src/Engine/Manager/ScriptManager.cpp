#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.h>
#include "../Util/Log.hpp"

void print(const std::string& message) {
    Log() << message;
}

ScriptManager::ScriptManager() {
    // Create the script engine
    engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);
    
    // Register add-ons.
    RegisterStdString(engine);
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
}

ScriptManager::~ScriptManager() {
    engine->ShutDownAndRelease();
}

void ScriptManager::TestScripting() {
    // Define the test script.
    std::string script = "void main() { print(\"Hello world\\n\"); }";
    
    // Create, load and build script module.
    asIScriptModule* module = engine->GetModule("module", asGM_ALWAYS_CREATE);
    module->AddScriptSection("script.as", script.c_str());
    int r = module->Build();
    if (r < 0)
        Log() << "Couldn't build script module.\n";
    
    // Find function to call.
    asIScriptFunction* function = module->GetFunctionByDecl("void main()");
    if (function == nullptr)
        Log() << "Couldn't find \"void main()\" function.\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(function);
    r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
          // An exception occurred, let the script writer know what happened so it can be corrected.
          Log() << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
        }
    }
    
    // Clean up.
    context->Release();
}
