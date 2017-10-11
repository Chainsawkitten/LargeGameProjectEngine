#include "ParticleEmitter.hpp"

#include "../Util/Json.hpp"

using namespace Component;

ParticleEmitter::ParticleEmitter() {
    
}

Json::Value ParticleEmitter::Save() const {
    Json::Value component;
    
    component["textureIndex"] = particleType.textureIndex;
    component["minVelocity"] = Json::SaveVec3(particleType.minVelocity);
    component["maxVelocity"] = Json::SaveVec3(particleType.maxVelocity);
    component["averageLifetime"] = particleType.averageLifetime;
    component["lifetimeVariance"] = particleType.lifetimeVariance;
    component["averageSize"] = Json::SaveVec2(particleType.averageSize);
    component["sizeVariance"] = Json::SaveVec2(particleType.sizeVariance);
    component["uniformScaling"] = particleType.uniformScaling;
    component["startAlpha"] = particleType.startAlpha;
    component["midAlpha"] = particleType.midAlpha;
    component["endAlpha"] = particleType.endAlpha;
    component["color"] = Json::SaveVec3(particleType.color);
    component["size"] = Json::SaveVec3(size);
    component["averageEmitTime"] = averageEmitTime;
    component["emitTimeVariance"] = emitTimeVariance;
    component["emitterType"] = emitterType;
    
    return component;
}
