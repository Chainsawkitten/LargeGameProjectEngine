#include "ScriptManager.hpp"

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptmath/scriptmath.h>
#include <scriptstdstring/scriptstdstring.h>
#include <Utility/Log.hpp>
#include <map>
#include <typeindex>
#include "../Util/FileSystem.hpp"
#include "../Util/Input.hpp"
#include "../Util/RayIntersection.hpp"
#include "../Util/MousePicking.hpp"
#include "../Hymn.hpp"
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Script.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Physics.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/SpotLight.hpp"
#include "../Input/Input.hpp"
#include "../Script/ScriptFile.hpp"
#include "MainWindow.hpp"

#include "Managers.hpp"
#include "DebugDrawingManager.hpp"
#include "PhysicsManager.hpp"
#include "ResourceManager.hpp"
#include "Component/Mesh.hpp"

using namespace Component;

void AngelScriptMessageCallback(const asSMessageInfo* message, void* param) {
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

void print(const std::string& message) {
    Log() << message;
}

void RegisterUpdate() {
    Managers().scriptManager->RegisterUpdate(Managers().scriptManager->currentEntity);
}

void RegisterTriggerHelper(Component::Physics* triggerBody, Component::Physics* object, const std::string& methodName) {
    Managers().scriptManager->RegisterTrigger(Managers().scriptManager->currentEntity, triggerBody, object, methodName);
}

bool ButtonInput(int buttonIndex) {
    return Input::GetInstance().CheckButton(buttonIndex);
}

glm::vec2 GetCursorXY() {
    return Input()->GetCursorXY();
}

void SendMessage(Entity* recipient, int type) {
    Managers().scriptManager->SendMessage(recipient, type);
}

bool IsIntersect(Entity* checker, Entity* camera) {
    MousePicking mousePicker = MousePicking(camera, camera->GetComponent<Component::Lens>()->GetProjection(glm::vec2(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y)));
    mousePicker.Update();
    RayIntersection rayIntersector;
    float intersectDistance;
    if (rayIntersector.RayOBBIntersect(camera->GetWorldPosition(), mousePicker.GetCurrentRay(),
        checker->GetComponent<Component::Mesh>()->geometry->GetAxisAlignedBoundingBox(),
        checker->GetModelMatrix(), intersectDistance)) {
        if (intersectDistance < 10.0f)
            return true;
        return false;
    }
    return false;
}

void vec2Constructor(float x, float y, void* memory) {
    glm::vec2* vec = static_cast<glm::vec2*>(memory);
    vec->x = x;
    vec->y = y;
}

void vec3Constructor(float x, float y, float z, void* memory) {
    glm::vec3* vec = static_cast<glm::vec3*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

void vec4Constructor(float x, float y, float z, float w, void* memory) {
    glm::vec4* vec = static_cast<glm::vec4*>(memory);
    vec->x = x;
    vec->y = y;
    vec->z = z;
    vec->w = w;
}

template<typename type> void glmConstructor(void* memory) {
    *static_cast<type*>(memory) = type();
}

template<typename type> type glmAdd(const type& a, const void* memory) {
    return *static_cast<const type*>(memory) + a;
}

template<typename type> type glmSub(const type& a, const void* memory) {
    return *static_cast<const type*>(memory) - a;
}

template<typename S, typename T> S glmMul(T a, const void* memory) {
    return *static_cast<const S*>(memory) * a;
}

template<typename type> type glmMulR(float a, const void* memory) {
    return a * *static_cast<const type*>(memory);
}

template<typename type> type glmDiv(float a, const void* memory) {
    return *static_cast<const type*>(memory) / a;
}

template<typename type> type glmDivR(float a, const void* memory) {
    return a / *static_cast<const type*>(memory);
}

template<typename type> type glmNeg(const void* memory) {
    return -*static_cast<const type*>(memory);
}

glm::vec3 mat3MulVec3(const glm::vec3& a, const void* memory) {
    return *static_cast<const glm::mat3*>(memory) * a;
}

glm::vec4 mat4MulVec4(const glm::vec4& a, const void* memory) {
    return *static_cast<const glm::mat4*>(memory) * a;
}

ScriptManager::ScriptManager() {
    // Create the script engine
    engine = asCreateScriptEngine();
    
    // Set the message callback to receive information on errors in human readable form.
    engine->SetMessageCallback(asFUNCTION(AngelScriptMessageCallback), 0, asCALL_CDECL);
    
    // Register add-ons.
    RegisterStdString(engine);
    RegisterScriptMath(engine);
    
    engine->RegisterEnum("input");
    
    // Register GLM types.
    engine->RegisterObjectType("vec2", sizeof(glm::vec2), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec2>());
    engine->RegisterObjectProperty("vec2", "float x", asOFFSET(glm::vec2, x));
    engine->RegisterObjectProperty("vec2", "float y", asOFFSET(glm::vec2, y));
    engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(vec2Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opAdd(const vec2 &in) const", asFUNCTIONPR(glmAdd, (const glm::vec2&, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opSub(const vec2 &in) const", asFUNCTIONPR(glmSub, (const glm::vec2&, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec2", "vec2& opAddAssign(const vec2 &in) const", asMETHODPR(glm::vec2, operator+=, (const glm::vec2&), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opSubAssign(const vec2 &in) const", asMETHODPR(glm::vec2, operator-=, (const glm::vec2&), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opMulAssign(float) const", asMETHODPR(glm::vec2, operator*=, (float), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2& opDivAssign(float) const", asMETHODPR(glm::vec2, operator/=, (float), glm::vec2&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec2", "vec2 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec2), asCALL_CDECL_OBJLAST);
    
    engine->RegisterObjectType("vec3", sizeof(glm::vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec3>());
    engine->RegisterObjectProperty("vec3", "float x", asOFFSET(glm::vec3, x));
    engine->RegisterObjectProperty("vec3", "float y", asOFFSET(glm::vec3, y));
    engine->RegisterObjectProperty("vec3", "float z", asOFFSET(glm::vec3, z));
    engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(vec3Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3 &in) const", asFUNCTIONPR(glmAdd, (const glm::vec3&, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opSub(const vec3 &in) const", asFUNCTIONPR(glmSub, (const glm::vec3&, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec3", "vec3& opAddAssign(const vec3 &in) const", asMETHODPR(glm::vec3, operator+=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opSubAssign(const vec3 &in) const", asMETHODPR(glm::vec3, operator-=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opMulAssign(float) const", asMETHODPR(glm::vec3, operator*=, (float), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3& opDivAssign(float) const", asMETHODPR(glm::vec3, operator/=, (float), glm::vec3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec3", "vec3 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec3), asCALL_CDECL_OBJLAST);
    
    engine->RegisterObjectType("vec4", sizeof(glm::vec4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::vec4>());
    engine->RegisterObjectProperty("vec4", "float x", asOFFSET(glm::vec4, x));
    engine->RegisterObjectProperty("vec4", "float y", asOFFSET(glm::vec4, y));
    engine->RegisterObjectProperty("vec4", "float z", asOFFSET(glm::vec4, z));
    engine->RegisterObjectProperty("vec4", "float w", asOFFSET(glm::vec4, w));
    engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(vec4Constructor), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opAdd(const vec4 &in) const", asFUNCTIONPR(glmAdd, (const glm::vec4&, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opSub(const vec4 &in) const", asFUNCTIONPR(glmSub, (const glm::vec4&, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("vec4", "vec4& opAddAssign(const vec4 &in) const", asMETHODPR(glm::vec4, operator+=, (const glm::vec4&), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opSubAssign(const vec4 &in) const", asMETHODPR(glm::vec4, operator-=, (const glm::vec4&), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opMulAssign(float) const", asMETHODPR(glm::vec4, operator*=, (float), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4& opDivAssign(float) const", asMETHODPR(glm::vec4, operator/=, (float), glm::vec4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("vec4", "vec4 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::vec4), asCALL_CDECL_OBJLAST);
    
    engine->RegisterObjectType("mat3", sizeof(glm::mat3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::mat3>());
    engine->RegisterObjectBehaviour("mat3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(glmConstructor<glm::mat3>, (void*), void), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opAdd(const mat3 &in) const", asFUNCTIONPR(glmAdd, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opSub(const mat3 &in) const", asFUNCTIONPR(glmSub, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opMul(const mat3 &in) const", asFUNCTIONPR(glmMul, (const glm::mat3&, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "mat3& opAddAssign(const mat3 &in) const", asMETHODPR(glm::mat3, operator+=, (const glm::mat3&), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opSubAssign(const mat3 &in) const", asMETHODPR(glm::mat3, operator-=, (const glm::mat3&), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opMulAssign(float) const", asMETHODPR(glm::mat3, operator*=, (float), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3& opDivAssign(float) const", asMETHODPR(glm::mat3, operator/=, (float), glm::mat3&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat3", "mat3 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::mat3), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat3", "vec3 opMul(const vec3 &in) const", asFUNCTION(mat3MulVec3), asCALL_CDECL_OBJLAST);
    
    engine->RegisterObjectType("mat4", sizeof(glm::mat4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<glm::mat4>());
    engine->RegisterObjectBehaviour("mat4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(glmConstructor<glm::mat4>, (void*), void), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opAdd(const mat4 &in) const", asFUNCTIONPR(glmAdd, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opSub(const mat4 &in) const", asFUNCTIONPR(glmSub, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul(float) const", asFUNCTIONPR(glmMul, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul_r(float) const", asFUNCTIONPR(glmMulR, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opMul(const mat4 &in) const", asFUNCTIONPR(glmMul, (const glm::mat4&, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opDiv(float) const", asFUNCTIONPR(glmDiv, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4 opDiv_r(float) const", asFUNCTIONPR(glmDivR, (float, const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "mat4& opAddAssign(const mat4 &in) const", asMETHODPR(glm::mat4, operator+=, (const glm::mat4&), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opSubAssign(const mat4 &in) const", asMETHODPR(glm::mat4, operator-=, (const glm::mat4&), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opMulAssign(float) const", asMETHODPR(glm::mat4, operator*=, (float), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4& opDivAssign(float) const", asMETHODPR(glm::mat4, operator/=, (float), glm::mat4&), asCALL_THISCALL);
    engine->RegisterObjectMethod("mat4", "mat4 opNeg() const", asFUNCTIONPR(glmNeg, (const void*), glm::mat4), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod("mat4", "vec4 opMul(const vec4 &in) const", asFUNCTION(mat4MulVec4), asCALL_CDECL_OBJLAST);
    
    // Register GLM functions.
    engine->RegisterGlobalFunction("vec2 normalize(const vec2 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec2&), glm::vec2), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec3 normalize(const vec3 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec3&), glm::vec3), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec4 normalize(const vec4 &in)", asFUNCTIONPR(glm::normalize, (const glm::vec4&), glm::vec4), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec2 &in)", asFUNCTIONPR(glm::length, (const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec3 &in)", asFUNCTIONPR(glm::length, (const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float length(const vec4 &in)", asFUNCTIONPR(glm::length, (const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec3 cross(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::cross, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec2 &in, const vec2 &in)", asFUNCTIONPR(glm::dot, (const glm::vec2&, const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::dot, (const glm::vec3&, const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float dot(const vec4 &in, const vec4 &in)", asFUNCTIONPR(glm::dot, (const glm::vec4&, const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float distance(const vec2 &in, const vec2 &in)", asFUNCTIONPR(glm::distance, (const glm::vec2&, const glm::vec2&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float distance(const vec3 &in, const vec3 &in)", asFUNCTIONPR(glm::distance, (const glm::vec3&, const glm::vec3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float distance(const vec4 &in, const vec4 &in)", asFUNCTIONPR(glm::distance, (const glm::vec4&, const glm::vec4&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat3 inverse(const mat3 &in)", asFUNCTIONPR(glm::inverse, (const glm::mat3&), glm::mat3), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat4 inverse(const mat4 &in)", asFUNCTIONPR(glm::inverse, (const glm::mat4&), glm::mat4), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat3 transpose(const mat3 &in)", asFUNCTIONPR(glm::transpose, (const glm::mat3&), glm::mat3), asCALL_CDECL);
    engine->RegisterGlobalFunction("mat4 transpose(const mat4 &in)", asFUNCTIONPR(glm::transpose, (const glm::mat4&), glm::mat4), asCALL_CDECL);
    engine->RegisterGlobalFunction("float determinant(const mat3 &in)", asFUNCTIONPR(glm::determinant, (const glm::mat3&), float), asCALL_CDECL);
    engine->RegisterGlobalFunction("float determinant(const mat4 &in)", asFUNCTIONPR(glm::determinant, (const glm::mat4&), float), asCALL_CDECL);
    
    // Register Entity.
    engine->RegisterObjectType("Entity", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Entity", "string name", asOFFSET(Entity, name));
    engine->RegisterObjectProperty("Entity", "vec3 position", asOFFSET(Entity, position));
    engine->RegisterObjectProperty("Entity", "vec3 scale", asOFFSET(Entity, scale));
    engine->RegisterObjectProperty("Entity", "vec3 rotation", asOFFSET(Entity, rotation));
    engine->RegisterObjectMethod("Entity", "void Kill()", asMETHOD(Entity, Kill), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsKilled() const", asMETHOD(Entity, IsKilled), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetParent() const", asMETHOD(Entity, GetParent), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ InstantiateScene(const string &in)", asMETHOD(Entity, InstantiateScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "bool IsScene() const", asMETHOD(Entity, IsScene), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Entity@ GetChild(const string &in) const", asMETHOD(Entity, GetChild), asCALL_THISCALL);
    
    // Register components.
    engine->SetDefaultNamespace("Component");
    
    engine->RegisterObjectType("DirectionalLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("DirectionalLight", "vec3 color", asOFFSET(DirectionalLight, color));
    engine->RegisterObjectProperty("DirectionalLight", "float ambientCoefficient", asOFFSET(DirectionalLight, ambientCoefficient));
    
    engine->RegisterObjectType("Lens", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Lens", "float fieldOfView", asOFFSET(Lens, fieldOfView));
    engine->RegisterObjectProperty("Lens", "float zNear", asOFFSET(Lens, zNear));
    engine->RegisterObjectProperty("Lens", "float zFar", asOFFSET(Lens, zFar));
    
    engine->RegisterObjectType("Listener", 0, asOBJ_REF | asOBJ_NOCOUNT);
    
    engine->RegisterObjectType("Physics", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Physics", "vec3 velocity", asOFFSET(Component::Physics, velocity));
    engine->RegisterObjectProperty("Physics", "float maxVelocity", asOFFSET(Component::Physics, maxVelocity));
    engine->RegisterObjectProperty("Physics", "vec3 angularVelocity", asOFFSET(Component::Physics, angularVelocity));
    engine->RegisterObjectProperty("Physics", "float maxAngularVelocity", asOFFSET(Component::Physics, maxAngularVelocity));
    engine->RegisterObjectProperty("Physics", "vec3 acceleration", asOFFSET(Component::Physics, acceleration));
    engine->RegisterObjectProperty("Physics", "vec3 angularAcceleration", asOFFSET(Component::Physics, angularAcceleration));
    engine->RegisterObjectProperty("Physics", "float velocityDragFactor", asOFFSET(Component::Physics, velocityDragFactor));
    engine->RegisterObjectProperty("Physics", "float angularDragFactor", asOFFSET(Component::Physics, angularDragFactor));
    engine->RegisterObjectProperty("Physics", "float gravityFactor", asOFFSET(Component::Physics, gravityFactor));
    engine->RegisterObjectProperty("Physics", "vec3 momentOfInertia", asOFFSET(Component::Physics, momentOfInertia));
    
    engine->RegisterObjectType("PointLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("PointLight", "vec3 color", asOFFSET(PointLight, color));
    engine->RegisterObjectProperty("PointLight", "float ambientCoefficient", asOFFSET(PointLight, ambientCoefficient));
    engine->RegisterObjectProperty("PointLight", "float attenuation", asOFFSET(PointLight, attenuation));
    engine->RegisterObjectProperty("PointLight", "float intensity", asOFFSET(PointLight, intensity));
    
    engine->RegisterObjectType("SpotLight", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SpotLight", "vec3 color", asOFFSET(SpotLight, color));
    engine->RegisterObjectProperty("SpotLight", "float ambientCoefficient", asOFFSET(SpotLight, ambientCoefficient));
    engine->RegisterObjectProperty("SpotLight", "float attenuation", asOFFSET(SpotLight, attenuation));
    engine->RegisterObjectProperty("SpotLight", "float intensity", asOFFSET(SpotLight, intensity));
    engine->RegisterObjectProperty("SpotLight", "float coneAngle", asOFFSET(SpotLight, coneAngle));
    
    engine->RegisterObjectType("SoundSource", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("SoundSource", "float pitch", asOFFSET(SoundSource, pitch));
    engine->RegisterObjectProperty("SoundSource", "float gain", asOFFSET(SoundSource, gain));
    engine->RegisterObjectProperty("SoundSource", "bool loop", asOFFSET(SoundSource, loop));
    engine->RegisterObjectMethod("SoundSource", "void Play()", asMETHOD(SoundSource, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Pause()", asMETHOD(SoundSource, Pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("SoundSource", "void Stop()", asMETHOD(SoundSource, Stop), asCALL_THISCALL);
    
    engine->SetDefaultNamespace("");
    
    // Register getting components.
    engine->RegisterObjectMethod("Entity", "Component::DirectionalLight@ GetDirectionalLight()", asMETHODPR(Entity, GetComponent, () const, DirectionalLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Lens@ GetLens()", asMETHODPR(Entity, GetComponent, () const, Lens*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Listener@ GetListener()", asMETHODPR(Entity, GetComponent, () const, Listener*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::Physics@ GetPhysics()", asMETHODPR(Entity, GetComponent, () const, Component::Physics*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::PointLight@ GetPointLight()", asMETHODPR(Entity, GetComponent, () const, PointLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SpotLight@ GetSpotLight()", asMETHODPR(Entity, GetComponent, () const, SpotLight*), asCALL_THISCALL);
    engine->RegisterObjectMethod("Entity", "Component::SoundSource@ GetSoundSource()", asMETHODPR(Entity, GetComponent, () const, SoundSource*), asCALL_THISCALL);
    
    // Register managers.
    engine->RegisterObjectType("DebugDrawingManager", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddPoint(const vec3 &in, const vec3 &in, float, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddPoint), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddLine(const vec3 &in, const vec3 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddLine), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddCuboid(const vec3 &in, const vec3 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddCuboid), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddPlane(const vec3 &in, const vec3 &in, const vec2 &in, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddPlane), asCALL_THISCALL);
    engine->RegisterObjectMethod("DebugDrawingManager", "void AddSphere(const vec3 &in, float, const vec3 &in, float = 1.0, float = 0.0, bool = true)", asMETHOD(DebugDrawingManager, AddSphere), asCALL_THISCALL);
    
    engine->RegisterObjectType("Hub", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectProperty("Hub", "DebugDrawingManager@ debugDrawingManager", asOFFSET(Hub, debugDrawingManager));
    
    // Register functions.
    engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterUpdate()", asFUNCTION(::RegisterUpdate), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RegisterTrigger(Component::Physics@, Component::Physics@, const string &in)", asFUNCTION(RegisterTriggerHelper), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool Input(input button)", asFUNCTION(ButtonInput), asCALL_CDECL);
    engine->RegisterGlobalFunction("void SendMessage(Entity@, int)", asFUNCTION(::SendMessage), asCALL_CDECL);
    engine->RegisterGlobalFunction("Hub@ Managers()", asFUNCTION(Managers), asCALL_CDECL);
    engine->RegisterGlobalFunction("vec2 GetCursorXY()", asFUNCTION(GetCursorXY), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool IsIntersect(Entity@, Entity@)", asFUNCTION(IsIntersect), asCALL_CDECL);
}

ScriptManager::~ScriptManager() {
    engine->ShutDownAndRelease();
}

void ScriptManager::BuildScript(const std::string& name) {
    std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER + name + ".as";
    if (!FileSystem::FileExists(filename.c_str())) {
        Log() << "Script file does not exist: " << filename << "\n";
        return;
    }
    
    // Create and build script module.
    CScriptBuilder builder;
    int r = builder.StartNewModule(engine, name.c_str());
    if (r < 0)
        Log() << "Couldn't start new module: " << name << ".\n";
    
    r = builder.AddSectionFromFile(filename.c_str());
    if (r < 0)
        Log() << "File section could not be added: " << filename << ".\n";
    
    r = builder.BuildModule();
    if (r < 0)
        Log() << "Compile errors.\n";
}

void ScriptManager::BuildAllScripts() {

    const std::vector<Component::Script*> scriptComponents = scripts.GetAll();

    for (int i = 0; i < scriptComponents.size(); i++) {

        if(scriptComponents[i]->instance != nullptr)
            scriptComponents[i]->instance->Release();

    }

    std::string path = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER;
    
    for (ScriptFile* file : Hymn().scripts) {
        std::string filename = path + file->name + ".as";
        if (!FileSystem::FileExists(filename.c_str())) {
            Log() << "Script file does not exist: " << filename << "\n";
            return;
        }
        
        // Create and build script module.
        CScriptBuilder builder;
        asIScriptModule* module = engine->GetModule(file->name.c_str());
        if (module == nullptr) {
            int r = builder.StartNewModule(engine, file->name.c_str());
            if (r < 0)
                Log() << "Couldn't start new module: " << file->name << ".\n";
            
            r = builder.AddSectionFromFile(filename.c_str());
            if (r < 0)
                Log() << "File section could not be added: " << filename << ".\n";

            r = builder.BuildModule();
            if (r < 0)
                Log() << "Compile errors.\n";

        } else {
            std::string script;

            LoadScriptFile(filename.c_str(), script);
            module->AddScriptSection(filename.c_str(), script.c_str());
            
            int r = module->Build();
            if (r < 0)
                Log() << file->name.c_str() << "Compile errors.\n";
        }
    }
}

void ScriptManager::FillPropertyMap(Script* script) {

    BuildScript(script->scriptFile->name);
    CreateInstance(script);

    for (auto &propertyMap : script->propertyMap) 
        propertyMap.second.clear();

    script->propertyMap.clear();
    int propertyCount = script->instance->GetPropertyCount();

    for (int n = 0; n < propertyCount; n++) {

        int typeId = script->instance->GetPropertyTypeId(n);
        void *varPointer = script->instance->GetAddressOfProperty(n);

        if (typeId == asTYPEID_INT32)
        {
            int* mapValue = new int();
            *mapValue = *(int*)varPointer;
            script->propertyMap[script->instance->GetPropertyName(n)][typeId] = mapValue;
        }
        else if (typeId == asTYPEID_FLOAT)
        {
            float* mapValue = new float();
            *mapValue = *(float*)varPointer;
            script->propertyMap[script->instance->GetPropertyName(n)][typeId] = mapValue;
        }
        else if (typeId == script->instance->GetEngine()->GetTypeIdByDecl("string"))
        {
            std::string *str = (std::string*)varPointer;
            if (str) {

                std::string* mapValue = new std::string();
                *mapValue = *(std::string*)varPointer;
                script->propertyMap[script->instance->GetPropertyName(n)][typeId] = mapValue;

            }

        }

    }

}

void ScriptManager::Update(World& world, float deltaTime) {
    // Init.
    for (Script* script : scripts.GetAll()) {
        if (!script->initialized && !script->IsKilled() && script->entity->enabled) {
            CreateInstance(script);
            script->initialized = true;

            int propertyCount = script->instance->GetPropertyCount();

            for (int n = 0; n < propertyCount; n++) {

                int typeId = script->instance->GetPropertyTypeId(n);
                void *varPointer = script->instance->GetAddressOfProperty(n);

                std::map<std::string, std::map<int, void*>>::iterator it = script->propertyMap.find(script->instance->GetPropertyName(n));

                if (it != script->propertyMap.end())
                {

                    std::map<int, void*>::iterator it2 = script->propertyMap[script->instance->GetPropertyName(n)].find(typeId);

                    if (it2 != script->propertyMap[script->instance->GetPropertyName(n)].end()) {

                        if (typeId == asTYPEID_INT32)
                        {
                            int* propertyPointer = static_cast<int*>(varPointer);
                            *propertyPointer = *(int*)script->propertyMap[script->instance->GetPropertyName(n)][typeId];
                        }
                        else if (typeId == asTYPEID_FLOAT)
                        {
                            float* propertyPointer = static_cast<float*>(varPointer);
                            *propertyPointer = *(float*)script->propertyMap[script->instance->GetPropertyName(n)][typeId];
                        }
                        else if (typeId == script->instance->GetEngine()->GetTypeIdByDecl("string"))
                        {

                            std::string *str = (std::string*)varPointer;
                            if (str) {



                            }

                        }

                    }
                }
            }
        }
    }
    
    // Update.
    for (Entity* entity : world.GetUpdateEntities())
        CallUpdate(entity, deltaTime);
    
    // Handle messages.
    while (!messages.empty()) {
        std::vector<Message> temp = messages;
        messages.clear();
        
        for (const Message& message : temp)
            CallMessageReceived(message);
    }
    
    // Register entities for events.
    for (Entity* entity : updateEntities)
        world.RegisterUpdate(entity);
    updateEntities.clear();
    
    // Handle physics triggers.
    for (const TriggerEvent& triggerEvent : triggerEvents) {
        CallTrigger(triggerEvent);
    }
    triggerEvents.clear();
}

void ScriptManager::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

void ScriptManager::RegisterTrigger(Entity* entity, Component::Physics* trigger, Component::Physics* object, const std::string& methodName) {
    TriggerEvent triggerEvent;
    triggerEvent.trigger = trigger;
    triggerEvent.object = object;
    triggerEvent.scriptEntity = entity;
    triggerEvent.methodName = methodName;
    
    Managers().physicsManager->OnTriggerEnter(trigger, object, std::bind(&ScriptManager::HandleTrigger, this, triggerEvent));
}

void ScriptManager::RegisterInput() {
    // Get the input enum.
    asUINT enumCount = engine->GetEnumCount();
    asITypeInfo* inputEnum = nullptr;
    for (asUINT i = 0; i < enumCount; ++i) {
        asITypeInfo* asEnum = engine->GetEnumByIndex(i);
        std::string name = asEnum->GetName();
        if (name == "input") {
            inputEnum = asEnum;
            break;
        }
    }
    
    for (std::size_t i = 0; i < Input::GetInstance().buttons.size(); ++i) {
        Input::Button* button = Input::GetInstance().buttons[i];
        
        // Check if we've already registered the button.
        bool registered = false;
        asUINT inputCount = inputEnum->GetEnumValueCount();
        for (asUINT j = 0; j < inputCount; ++j) {
            int value;
            std::string registeredButton = inputEnum->GetEnumValueByIndex(j, &value);
            if (registeredButton == button->action) {
                registered = true;
                break;
            }
        }
        
        if (!registered)
            engine->RegisterEnumValue("input", std::string(button->action).c_str(), i);
    }
}

void ScriptManager::SendMessage(Entity* recipient, int type) {
    Message message;
    message.recipient = recipient;
    message.type = type;
    messages.push_back(message);
}

Entity* ScriptManager::GetEntity(unsigned int GUID) const {

    const std::vector<Entity*> entities = Hymn().world.GetEntities();
    for (int i = 0; i < entities.size(); i++) {

        if (entities[i]->GetUniqueIdentifier() == GUID) {

            return entities[i];

        }

    }

}

Component::Script* ScriptManager::CreateScript() {
    return scripts.Create();
}

Component::Script* ScriptManager::CreateScript(const Json::Value& node) {
    Component::Script* script = scripts.Create();
    
    // Load values from Json node.
    std::string name = node.get("scriptName", "").asString();
    script->scriptFile = Managers().resourceManager->CreateScriptFile(name);

    Json::Value propertyMapJson = node.get("propertyMap", "");

    if (propertyMapJson.asString() != "") {

        std::vector<std::string> names = propertyMapJson.getMemberNames();

        for (auto name : names) {

            Log() << name << "\n";

        }

    }
    
    return script;
}

const std::vector<Component::Script*>& ScriptManager::GetScripts() const {
    return scripts.GetAll();
}

void ScriptManager::ClearKilledComponents() {
    scripts.ClearKilled();
}

void ScriptManager::CreateInstance(Component::Script* script) {
    currentEntity = script->entity;
    ScriptFile* scriptFile = script->scriptFile;
    
    // Find the class to instantiate.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find factory function / constructor.
    std::string factoryName = scriptFile->name + "@ " + scriptFile->name + "(Entity@)";
    asIScriptFunction* factoryFunction = type->GetFactoryByDecl(factoryName.c_str());
    if (factoryFunction == nullptr)
        Log() << "Couldn't find the factory function for " << scriptFile->name << ".\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(factoryFunction);
    context->SetArgObject(0, script->entity);
    ExecuteCall(context);
    
    // Get the newly created object.
    script->instance = *(static_cast<asIScriptObject**>(context->GetAddressOfReturnValue()));
    script->instance->AddRef();

    // Clean up.
    context->Release();
}

void ScriptManager::CallMessageReceived(const Message& message) {
    currentEntity = message.recipient;
    Component::Script* script = currentEntity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl("void ReceiveMessage(int)");
    if (method == nullptr)
        Log() << "Can't find method void ReceiveMessage(int)\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgDWord(0, message.type);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::CallUpdate(Entity* entity, float deltaTime) {
    Component::Script* script = entity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    asIScriptFunction* method = type->GetMethodByDecl("void Update(float)");
    if (method == nullptr)
        Log() << "Can't find method void Update(float)\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgFloat(0, deltaTime);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::CallTrigger(const TriggerEvent& triggerEvent) {
    Component::Script* script = triggerEvent.scriptEntity->GetComponent<Component::Script>();
    ScriptFile* scriptFile = script->scriptFile;
    
    // Get class.
    asITypeInfo* type = GetClass(scriptFile->name, scriptFile->name);
    
    // Find method to call.
    std::string methodDeclaration = "void " + triggerEvent.methodName + "(Component::Physics@, Component::Physics@)";
    asIScriptFunction* method = type->GetMethodByDecl(methodDeclaration.c_str());
    if (method == nullptr)
        Log() << "Can't find method " << methodDeclaration << "\n";
    
    // Create context, prepare it and execute.
    asIScriptContext* context = engine->CreateContext();
    context->Prepare(method);
    context->SetObject(script->instance);
    context->SetArgAddress(0, triggerEvent.trigger);
    context->SetArgAddress(1, triggerEvent.object);
    ExecuteCall(context);
    
    // Clean up.
    context->Release();
}

void ScriptManager::LoadScriptFile(const char* fileName, std::string& script){
    // Open the file in binary mode
    FILE* f = fopen(fileName, "rb");
    
    // Determine the size of the file
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // Load the entire file in one call
    script.resize(len);
    fread(&script[0], len, 1, f);
    
    fclose(f);
}

void ScriptManager::ExecuteCall(asIScriptContext* context) {
    int r = context->Execute();
    if (r != asEXECUTION_FINISHED) {
        // The execution didn't complete as expected. Determine what happened.
        if (r == asEXECUTION_EXCEPTION) {
            // An exception occurred, let the script writer know what happened so it can be corrected.
            Log() << "An exception '" << context->GetExceptionString() << "' occurred. Please correct the code and try again.\n";
        }
    }
}

asITypeInfo* ScriptManager::GetClass(const std::string& moduleName, const std::string& className) {
    // Get script module.
    asIScriptModule* module = engine->GetModule(moduleName.c_str(), asGM_ONLY_IF_EXISTS);
    if (module == nullptr)
        Log() << "Couldn't find \"" << moduleName << "\" module.\n";
    
    // Find the class.
    asUINT typeCount = module->GetObjectTypeCount();
    for (asUINT i = 0; i < typeCount; ++i) {
        asITypeInfo* type = module->GetObjectTypeByIndex(i);
        if (strcmp(type->GetName(), className.c_str()) == 0)
            return type;
    }
    
    Log() << "Couldn't find class \"" << className << "\".\n";
    return nullptr;
}

void ScriptManager::HandleTrigger(TriggerEvent triggerEvent) {
    triggerEvents.push_back(triggerEvent);
}
