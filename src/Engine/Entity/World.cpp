#include "World.hpp"

#include "../Entity/Entity.hpp"
#include "../Component/SuperComponent.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ParticleManager.hpp"
#include "../Util/FileSystem.hpp"
#include "Hymn.hpp"
#include <fstream>
#include <ctime>

World::World() {
    particles = new Video::ParticleRenderer::Particle[Managers().particleManager->GetMaxParticleCount()];
}

World::~World() {
    Clear();
    
    delete[] particles;
}

Entity* World::CreateEntity(const std::string& name) {
    Entity* entity = new Entity(this, name);
    entities.push_back(entity);
    entity->SetUniqueIdentifier(std::time(NULL));
    return entity;
}

const std::vector<Entity*>& World::GetEntities() const {
    return entities;
}

void World::CreateRoot() {
    root = CreateEntity("Root");
}

Entity* World::GetRoot() const {
    return root;
}

void World::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

const std::vector<Entity*>& World::GetUpdateEntities() const {
    return updateEntities;
}

void World::Clear() {
    for (Entity* entity : entities)
        delete entity;
    entities.clear();
    root = nullptr;
    
    Managers().ClearComponents(this);

    particleCount = 0;
    updateEntities.clear();
}

void World::ClearKilled() {

    // Clear killed components.
    Managers().ClearKilledComponents(this);

    // Clear killed entities.
    std::size_t i = 0;
    while (i < entities.size()) {
        if (entities[i]->IsKilled()) {
            delete entities[i];
            entities[i] = entities[entities.size() - 1];
            entities.pop_back();
        } else {
            ++i;
        }
    }
}

Video::ParticleRenderer::Particle* World::GetParticles() const {
    return particles;
}

unsigned int World::GetParticleCount() const {
    return particleCount;
}

void World::SetParticleCount(unsigned int particleCount) {
    this->particleCount = particleCount;
}

void World::Save(const std::string& filename) const {
    Json::Value rootNode = root->Save();
    
    std::ofstream file(filename);
    file << rootNode;
    file.close();
}

void World::Load(const std::string& filename) {
    Clear();

    CreateRoot();
    
    // Load Json document from file.
    if (FileSystem::FileExists(filename.c_str())) {
        Json::Value rootNode;
        std::ifstream file(filename);
        file >> rootNode;
        file.close();
        
        root->Load(rootNode);
    }
}