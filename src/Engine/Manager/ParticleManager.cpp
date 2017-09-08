#include "ParticleManager.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Component/Lens.hpp"
#include "../MainWindow.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include <Video/Texture/Texture2D.hpp>
#include "ParticleAtlas.png.hpp"
#include <Video/ParticleRenderer.hpp>

using namespace Video;

ParticleManager::ParticleManager() {
    randomEngine.seed(randomDevice());
    
    textureAtlas = Managers().resourceManager->CreateTexture2D(PARTICLEATLAS_PNG, PARTICLEATLAS_PNG_LENGTH);
    
    particleRenderer = new ParticleRenderer(maxParticleCount);
}

ParticleManager::~ParticleManager() {
    delete particleRenderer;
    
    Managers().resourceManager->FreeTexture2D(textureAtlas);    
}

unsigned int ParticleManager::GetMaxParticleCount() const {
    return maxParticleCount;
}

void ParticleManager::Update(World& world, float time, bool preview) {
    // Remove old particles.
    for (unsigned int i = 0; i < world.GetParticleCount(); ++i) {
        world.GetParticles()[i].life += time;
        if (world.GetParticles()[i].life >= world.GetParticles()[i].lifetime) {
            world.GetParticles()[i--] = world.GetParticles()[world.GetParticleCount() - 1];
            world.SetParticleCount(world.GetParticleCount() - 1);
        }
    }
    
    // Spawn new particles from emitters.
    std::uniform_real_distribution<float> minusOneToOne(-1.f, 1.f);
    std::vector<Component::ParticleEmitter*> particleEmitters = world.GetComponents<Component::ParticleEmitter>();
    for (Component::ParticleEmitter* emitter : particleEmitters) {
        if (emitter->IsKilled() || (preview && !emitter->preview))
            continue;
        
        emitter->timeToNext -= time;
        while (emitter->timeToNext < 0.f) {
            emitter->timeToNext += emitter->averageEmitTime + minusOneToOne(randomEngine) * emitter->emitTimeVariance;
            EmitParticle(world, emitter);
        }
    }
}

void ParticleManager::UpdateBuffer(World& world) {
    particleRenderer->SetBufferContents(world.GetParticleCount(), world.GetParticles());
}

void ParticleManager::Render(World& world, const Entity* camera) {
    if (world.GetParticleCount() > 0) {
        glm::mat4 viewMatrix = camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetWorldPosition());
        glm::mat4 projectionMatrix = camera->GetComponent<Component::Lens>()->GetProjection(MainWindow::GetInstance()->GetSize());
        glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
        glm::vec3 up(glm::inverse(camera->GetCameraOrientation())* glm::vec4(0, 1, 0, 1));
        
        particleRenderer->Render(textureAtlas, textureAtlasRowNumber, camera->GetWorldPosition(), up, viewProjectionMatrix);
    }
}

const Texture2D* ParticleManager::GetTextureAtlas() const {
    return textureAtlas;
}

int ParticleManager::GetTextureAtlasRows() const {
    return textureAtlasRowNumber;
}

void ParticleManager::EmitParticle(World& world, Component::ParticleEmitter* emitter) {
    glm::vec3 position(emitter->entity->GetWorldPosition());
    if (emitter->emitterType == Component::ParticleEmitter::CUBOID) {
        std::uniform_real_distribution<float> randomSpread(-0.5f, 0.5f);
        glm::vec3 random(randomSpread(randomEngine), randomSpread(randomEngine), randomSpread(randomEngine));
        position += random * emitter->size;
    }
    EmitParticle(world, position, emitter);
}

void ParticleManager::EmitParticle(World& world, const glm::vec3& position, Component::ParticleEmitter* emitter) {
    if (world.GetParticleCount() < maxParticleCount) {
        Video::ParticleRenderer::Particle particle;
        std::uniform_real_distribution<float> zeroToOne(0.f, 1.f);
        std::uniform_real_distribution<float> minusOneToOne(-1.f, 1.f);
        particle.worldPos = position;
        particle.life = 0.f;
        particle.lifetime = emitter->particleType.averageLifetime + minusOneToOne(randomEngine) * emitter->particleType.lifetimeVariance;
        particle.textureIndex = static_cast<float>(emitter->particleType.textureIndex);
        particle.alpha[0] = emitter->particleType.startAlpha;
        particle.alpha[1] = emitter->particleType.midAlpha;
        particle.alpha[2] = emitter->particleType.endAlpha;
        particle.color = emitter->particleType.color;
        
        if (emitter->particleType.uniformScaling) {
            particle.size = emitter->particleType.averageSize + minusOneToOne(randomEngine) * emitter->particleType.sizeVariance;
        } else {
            particle.size.x = emitter->particleType.averageSize.x + minusOneToOne(randomEngine) * emitter->particleType.sizeVariance.x;
            particle.size.y = emitter->particleType.averageSize.y + minusOneToOne(randomEngine) * emitter->particleType.sizeVariance.y;
        }
        
        particle.velocity.x = emitter->particleType.minVelocity.x + zeroToOne(randomEngine) * (emitter->particleType.maxVelocity.x - emitter->particleType.minVelocity.x);
        particle.velocity.y = emitter->particleType.minVelocity.y + zeroToOne(randomEngine) * (emitter->particleType.maxVelocity.y - emitter->particleType.minVelocity.y);
        particle.velocity.z = emitter->particleType.minVelocity.z + zeroToOne(randomEngine) * (emitter->particleType.maxVelocity.z - emitter->particleType.minVelocity.z);
        
        world.GetParticles()[world.GetParticleCount()] = particle;
        world.SetParticleCount(world.GetParticleCount() + 1);
    }
}
