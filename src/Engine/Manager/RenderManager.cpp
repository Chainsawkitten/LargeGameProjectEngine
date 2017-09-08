#include "RenderManager.hpp"

#include <Video/Renderer.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "ParticleManager.hpp"
#include "Light.png.hpp"
#include "ParticleEmitter.png.hpp"
#include "SoundSource.png.hpp"
#include "Camera.png.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/SoundSource.hpp"
#include <Video/Geometry/Geometry3D.hpp>
#include "../Geometry/RiggedModel.hpp"
#include "../Geometry/Skeleton.hpp"
#include "../Texture/TextureAsset.hpp"
#include <Video/Texture/Texture2D.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Video/Culling/Frustum.hpp>
#include <Video/Culling/AxisAlignedBoundingBox.hpp>
#include "../MainWindow.hpp"
#include <Video/Lighting/Light.hpp>
#include <Video/RenderTarget.hpp>
#include "../Hymn.hpp"

using namespace Component;

RenderManager::RenderManager() {
    renderer = new Video::Renderer(MainWindow::GetInstance()->GetSize());
    
    // Init textures.
    particleEmitterTexture = Managers().resourceManager->CreateTexture2D(PARTICLEEMITTER_PNG, PARTICLEEMITTER_PNG_LENGTH);
    lightTexture = Managers().resourceManager->CreateTexture2D(LIGHT_PNG, LIGHT_PNG_LENGTH);
    soundSourceTexture = Managers().resourceManager->CreateTexture2D(SOUNDSOURCE_PNG, SOUNDSOURCE_PNG_LENGTH);
    cameraTexture = Managers().resourceManager->CreateTexture2D(CAMERA_PNG, CAMERA_PNG_LENGTH);
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeTexture2D(particleEmitterTexture);
    Managers().resourceManager->FreeTexture2D(lightTexture);
    Managers().resourceManager->FreeTexture2D(soundSourceTexture);
    Managers().resourceManager->FreeTexture2D(cameraTexture);
    
    delete renderer;
}

void RenderManager::Render(World& world, Entity* camera) {
    renderer->Clear();
    
    // Find camera entity.
    if (camera == nullptr) {
        std::vector<Lens*> lenses = world.GetComponents<Lens>();
        for (Lens* lens : lenses) {
            camera = lens->entity;
        }
    }
    
    // Render from camera.
    if (camera != nullptr) {
        glm::vec2 screenSize = MainWindow::GetInstance()->GetSize();
        renderer->StartRendering();
        
        // Camera matrices.
        glm::mat4 viewMatrix = camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetWorldPosition());
        glm::mat4 projectionMatrix = camera->GetComponent<Lens>()->GetProjection(screenSize);
        
        std::vector<Mesh*> meshes = world.GetComponents<Mesh>();
        
        // Render static meshes.
        renderer->PrepareStaticMeshRendering(viewMatrix, projectionMatrix);
        for (Mesh* mesh : meshes) {
            if (mesh->geometry != nullptr && mesh->geometry->GetType() == Video::Geometry::Geometry3D::STATIC) {
                Entity* entity = mesh->entity;
                Material* material = entity->GetComponent<Material>();
                if (material != nullptr) {
                    glm::mat4 modelMatrix = entity->GetModelMatrix();
                    renderer->RenderStaticMesh(mesh->geometry, material->diffuse->GetTexture(), material->normal->GetTexture(), material->specular->GetTexture(), material->glow->GetTexture(), modelMatrix);
                }
            }
        }
        
        // Render skinned meshes.
        renderer->PrepareSkinnedMeshRendering(viewMatrix, projectionMatrix);
        for (Mesh* mesh : meshes) {
            if (mesh->geometry != nullptr && mesh->geometry->GetType() == Video::Geometry::Geometry3D::SKIN) {
                Entity* entity = mesh->entity;
                Material* material = entity->GetComponent<Material>();
                if (material != nullptr) {
                    glm::mat4 modelMatrix = entity->GetModelMatrix();
                    Geometry::RiggedModel* model = static_cast<Geometry::RiggedModel*>(mesh->geometry);
                    renderer->RenderSkinnedMesh(mesh->geometry, material->diffuse->GetTexture(), material->normal->GetTexture(), material->specular->GetTexture(), material->glow->GetTexture(), modelMatrix, model->skeleton.GetFinalTransformations(), model->skeleton.GetFinalTransformationsIT());
                }
            }
        }
        
        // Light the world.
        LightWorld(world, camera);
        
        // Anti-aliasing.
        if (Hymn().filterSettings.fxaa)
            renderer->AntiAlias();
        
        // Fog.
        if (Hymn().filterSettings.fog)
            renderer->RenderFog(camera->GetComponent<Component::Lens>()->GetProjection(screenSize), Hymn().filterSettings.fogDensity, Hymn().filterSettings.fogColor);
        
        // Render particles.
        Managers().particleManager->UpdateBuffer(world);
        Managers().particleManager->Render(world, camera);
        
        // Glow.
        if (Hymn().filterSettings.glow)
            renderer->ApplyGlow(Hymn().filterSettings.glowBlurAmount);
        
        // Color.
        if (Hymn().filterSettings.color)
            renderer->ApplyColorFilter(Hymn().filterSettings.colorColor);
        
        // Gamma correction.
        renderer->GammaCorrect();
        
        // Render to back buffer.
        renderer->DisplayResults(true);
    }
}

void RenderManager::RenderEditorEntities(World& world, Entity* camera, bool soundSources, bool particleEmitters, bool lightSources, bool cameras) {
    // Find camera entity.
    if (camera == nullptr) {
        std::vector<Lens*> lenses = world.GetComponents<Lens>();
        for (Lens* lens : lenses) {
            camera = lens->entity;
        }
    }
    
    // Render from camera.
    if (camera != nullptr) {
        glm::vec2 screenSize(MainWindow::GetInstance()->GetSize());
        glm::mat4 viewMat(camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetWorldPosition()));
        glm::mat4 projectionMat(camera->GetComponent<Lens>()->GetProjection(screenSize));
        glm::mat4 viewProjectionMatrix(projectionMat * viewMat);
        glm::vec3 up(glm::inverse(camera->GetCameraOrientation())* glm::vec4(0, 1, 0, 1));
        
        renderer->PrepareRenderingIcons(viewProjectionMatrix, camera->GetWorldPosition(), up);
        
        // Render sound sources.
        if (soundSources) {
            for (SoundSource* soundSource : world.GetComponents<SoundSource>())
                renderer->RenderIcon(soundSource->entity->GetWorldPosition(), soundSourceTexture);
        }
        
        // Render particle emitters.
        if (particleEmitters) {
            for (ParticleEmitter* emitter : world.GetComponents<ParticleEmitter>())
                renderer->RenderIcon(emitter->entity->GetWorldPosition(), particleEmitterTexture);
        }
        
        // Render light sources.
        if (lightSources) {
            for (DirectionalLight* light : world.GetComponents<DirectionalLight>())
                renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
            
            for (PointLight* light : world.GetComponents<PointLight>())
                renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
            
            for (SpotLight* light : world.GetComponents<SpotLight>())
                renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
        }
        
        // Render cameras.
        if (cameras) {
            for (Lens* lens : world.GetComponents<Lens>())
                renderer->RenderIcon(lens->entity->GetWorldPosition(), cameraTexture);
        }
        
        renderer->StopRenderingIcons();
    }
}

void RenderManager::UpdateBufferSize() {
    renderer->SetScreenSize(MainWindow::GetInstance()->GetSize());
}

void RenderManager::LightWorld(World& world, const Entity* camera) {
    // Get the camera matrices.
    glm::mat4 viewMat(camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetWorldPosition()));
    glm::mat4 projectionMat(camera->GetComponent<Component::Lens>()->GetProjection(MainWindow::GetInstance()->GetSize()));
    glm::mat4 viewProjectionMat(projectionMat * viewMat);
    
    float cutOff;
    Video::AxisAlignedBoundingBox aabb(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));
    
    // Add all directional lights.
    std::vector<Component::DirectionalLight*>& directionalLights = world.GetComponents<Component::DirectionalLight>();
    for (Component::DirectionalLight* directionalLight : directionalLights) {
        Entity* lightEntity = directionalLight->entity;
        glm::vec4 direction(glm::vec4(lightEntity->GetDirection(), 0.f));
        Video::Light light;
        light.position = viewMat * -direction;
        light.intensities = directionalLight->color;
        light.attenuation = 1.f;
        light.ambientCoefficient = directionalLight->ambientCoefficient;
        light.coneAngle = 0.f;
        light.direction = glm::vec3(0.f, 0.f, 0.f);
        renderer->AddLight(light);
    }
    
    // Add all spot lights.
    std::vector<Component::SpotLight*>& spotLights = world.GetComponents<Component::SpotLight>();
    for (Component::SpotLight* spotLight : spotLights) {
        Entity* lightEntity = spotLight->entity;
        glm::vec4 direction(viewMat * glm::vec4(lightEntity->GetDirection(), 0.f));
        glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
        Video::Light light;
        light.position = viewMat * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0));
        light.intensities = spotLight->color * spotLight->intensity;
        light.attenuation = spotLight->attenuation;
        light.ambientCoefficient = spotLight->ambientCoefficient;
        light.coneAngle = spotLight->coneAngle;
        light.direction = glm::vec3(direction);
        renderer->AddLight(light);
    }
    
    // At which point lights should be cut off (no longer contribute).
    cutOff = 0.0001f;
    
    // Add all point lights.
    std::vector<Component::PointLight*>& pointLights = world.GetComponents<Component::PointLight>();
    for (Component::PointLight* pointLight : pointLights) {
        Entity* lightEntity = pointLight->entity;
        float scale = sqrt((1.f / cutOff - 1.f) / pointLight->attenuation);
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->GetWorldPosition()) * glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f) * scale);
        
        Video::Frustum frustum(viewProjectionMat * modelMat);
        if (frustum.Collide(aabb)) {
            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            Video::Light light;
            light.position = viewMat * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0));
            light.intensities = pointLight->color * pointLight->intensity;
            light.attenuation = pointLight->attenuation;
            light.ambientCoefficient = pointLight->ambientCoefficient;
            light.coneAngle = 180.f;
            light.direction = glm::vec3(1.f, 0.f, 0.f);
            renderer->AddLight(light);
        }
    }
    
    // Render lights.
    renderer->Light(glm::inverse(projectionMat));
}
