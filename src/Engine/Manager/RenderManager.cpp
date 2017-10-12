#include "RenderManager.hpp"

#include <Video/FrameBuffer.hpp>
#include <Video/Renderer.hpp>
#include <Video/RenderSurface.hpp>
#include <Video/ReadWriteTexture.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "ParticleManager.hpp"
#include "SoundManager.hpp"
#include "PhysicsManager.hpp"
#include "DebugDrawingManager.hpp"
#include "Light.png.hpp"
#include "ParticleEmitter.png.hpp"
#include "SoundSource.png.hpp"
#include "Camera.png.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Animation.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/Shape.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/SoundSource.hpp"
#include "../Physics/Shape.hpp"
#include <Video/Geometry/Geometry3D.hpp>
#include "../Texture/TextureAsset.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <Video/Culling/Frustum.hpp>
#include <Video/Culling/AxisAlignedBoundingBox.hpp>
#include "../MainWindow.hpp"
#include <Video/Lighting/Light.hpp>
#include "../Hymn.hpp"
#include "Util/Profiling.hpp"
#include "Util/Json.hpp"
#include "Util/GPUProfiling.hpp"

#include "Manager/VRManager.hpp"

using namespace Component;

RenderManager::RenderManager() {
    renderer = new Video::Renderer();
    
    // Render surface for main window.
    mainWindowRenderSurface = new Video::RenderSurface(MainWindow::GetInstance()->GetSize());

    // Render surface for hmd.
    hmdRenderSurface = nullptr;
    if (Managers().vrManager->Active())
        hmdRenderSurface = new Video::RenderSurface(Managers().vrManager->GetRecommendedRenderTargetSize());

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
    
    delete mainWindowRenderSurface;

    if (hmdRenderSurface != nullptr)
        delete hmdRenderSurface;

    delete renderer;
}

void RenderManager::Render(World& world, Entity* camera) {
    renderer->Clear();

    // Find camera entity.
    if (camera == nullptr) {
        for (Lens* lens : lenses.GetAll())
            camera = lens->entity;
    }

    if (camera != nullptr) {
        // Render main window.
        if (mainWindowRenderSurface != nullptr) {
            { PROFILE("Render main window");
            { GPUPROFILE("Render main window", Video::Query::Type::TIME_ELAPSED);

                const glm::mat4 translationMat = glm::translate(glm::mat4(), -camera->GetWorldPosition());
                const glm::mat4 orientationMat = camera->GetCameraOrientation();
                const glm::mat4 projectionMat = camera->GetComponent<Lens>()->GetProjection(mainWindowRenderSurface->GetSize());

                Render(world, translationMat, orientationMat, projectionMat, mainWindowRenderSurface);
            }
            }
        }

        // Render hmd.
        if (hmdRenderSurface != nullptr) {
            { PROFILE("Render main hmd");
            { GPUPROFILE("Render main hmd", Video::Query::Type::TIME_ELAPSED);

                for (int i = 0; i < 2; ++i) {
                    vr::Hmd_Eye nEye = i == 0 ? vr::Eye_Left : vr::Eye_Right;

                    glm::vec3 position = camera->GetWorldPosition();
                    Lens* lens = camera->GetComponent<Lens>();
                    const glm::mat4 projectionMat = Managers().vrManager->GetHMDProjectionMatrix(nEye, lens->zNear, lens->zFar);

                    glm::mat4 hmdTransform = Managers().vrManager->GetHMDPoseMatrix();
                    glm::mat4 eyeTranslation = Managers().vrManager->GetHMDEyeToHeadMatrix(nEye);

                    glm::vec3 right = glm::vec3(hmdTransform[0][0], hmdTransform[1][0], hmdTransform[2][0]);
                    glm::vec3 up = glm::vec3(hmdTransform[0][1], hmdTransform[1][1], hmdTransform[2][1]);
                    glm::vec3 forward = glm::vec3(hmdTransform[0][2], hmdTransform[1][2], hmdTransform[2][2]);
                    glm::mat4 lensTranslation = glm::translate(glm::mat4(), -position);

                    glm::mat4 orientationMat = glm::transpose(glm::mat4(
                        glm::vec4(right, 0.f),
                        glm::vec4(up, 0.f),
                        glm::vec4(forward, 0.f),
                        glm::vec4(0.f, 0.f, 0.f, 1.f)
                    ));

                    glm::mat4 hmdTranslationLocal = glm::inverse(orientationMat) * hmdTransform;
                    glm::vec3 hmdPositionLocal = glm::vec3(hmdTranslationLocal[3][0], hmdTranslationLocal[3][1], hmdTranslationLocal[3][2]);
                    glm::vec3 hmdPositionScaled = hmdPositionLocal * Managers().vrManager->GetScale();
                    glm::mat4 hmdTranslationScaled = glm::translate(glm::mat4(), hmdPositionScaled);

                    glm::mat4 translationMat = eyeTranslation * hmdTranslationScaled * lensTranslation;

                    Render(world, translationMat, orientationMat, projectionMat, hmdRenderSurface);

                    hmdRenderSurface->Swap();
                    vr::Texture_t texture = { (void*)(std::uintptr_t)hmdRenderSurface->GetColorTexture()->GetTexture(), vr::TextureType_OpenGL, vr::ColorSpace_Auto };

                    // Submit texture to HMD.
                    Managers().vrManager->Submit(nEye, &texture);
                }
            }
            }

            { PROFILE("Sync hmd");
            { GPUPROFILE("Sync hmd", Video::Query::Type::TIME_ELAPSED);
                Managers().vrManager->Sync();
            }
            }
        }

        // Render to back buffer.
        { PROFILE("Render to back buffer");
        { GPUPROFILE("Render to back buffer", Video::Query::Type::TIME_ELAPSED);
        { GPUPROFILE("Render to back buffer", Video::Query::Type::SAMPLES_PASSED);
            renderer->DisplayResults(mainWindowRenderSurface, true);
        }
        }
        }
    }
}

void RenderManager::RenderEditorEntities(World& world, Entity* camera, bool soundSources, bool particleEmitters, bool lightSources, bool cameras, bool physics) {
    // Find camera entity.
    if (camera == nullptr) {
        for (Lens* lens : lenses.GetAll()) {
            camera = lens->entity;
        }
    }
    
    // Render from camera.
    if (camera != nullptr) {
        const glm::vec2 screenSize(MainWindow::GetInstance()->GetSize());
        const glm::mat4 viewMat(camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetWorldPosition()));
        const glm::mat4 projectionMat(camera->GetComponent<Lens>()->GetProjection(screenSize));
        const glm::mat4 viewProjectionMatrix(projectionMat * viewMat);
        const glm::vec3 up(glm::inverse(camera->GetCameraOrientation()) * glm::vec4(0, 1, 0, 1));
        
        renderer->PrepareRenderingIcons(viewProjectionMatrix, camera->GetWorldPosition(), up);
        
        // Render sound sources.
        if (soundSources) {
            for (SoundSource* soundSource : Managers().soundManager->GetSoundSources())
                renderer->RenderIcon(soundSource->entity->GetWorldPosition(), soundSourceTexture);
        }
        
        // Render particle emitters.
        if (particleEmitters) {
            for (ParticleEmitter* emitter : Managers().particleManager->GetParticleEmitters())
                renderer->RenderIcon(emitter->entity->GetWorldPosition(), particleEmitterTexture);
        }
        
        // Render light sources.
        if (lightSources) {
            for (DirectionalLight* light : directionalLights.GetAll())
                renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
            
            for (PointLight* light : pointLights.GetAll())
                renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
            
            for (SpotLight* light : spotLights.GetAll())
                renderer->RenderIcon(light->entity->GetWorldPosition(), lightTexture);
        }
        
        // Render cameras.
        if (cameras) {
            for (Lens* lens : lenses.GetAll())
                renderer->RenderIcon(lens->entity->GetWorldPosition(), cameraTexture);
        }
        
        renderer->StopRenderingIcons();
        
        // Render physics.
        if (physics) {
            for (Component::Shape* shapeComp : Managers().physicsManager->GetShapeComponents()) {
                const ::Physics::Shape& shape = *shapeComp->GetShape();
                if (shape.GetKind() == ::Physics::Shape::Kind::Sphere) {
                    Managers().debugDrawingManager->AddSphere(shapeComp->entity->position, shape.GetSphereData()->radius, glm::vec3(1.0f, 1.0f, 1.0f));
                } else if (shape.GetKind() == ::Physics::Shape::Kind::Plane) {
                    Managers().debugDrawingManager->AddPlane(shapeComp->entity->position, shape.GetPlaneData()->normal, glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
                }
            }
        }
    }
}

void RenderManager::UpdateBufferSize() {
    delete mainWindowRenderSurface;

    mainWindowRenderSurface = new Video::RenderSurface(MainWindow::GetInstance()->GetSize());
}

void RenderManager::Render(World& world, const glm::mat4& translationMatrix, const glm::mat4& orientationMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface) {
    // Render from camera.
    renderer->StartRendering(renderSurface);

    // Camera matrices.
    const glm::vec3 position = glm::vec3(translationMatrix[3][0], translationMatrix[3][1], translationMatrix[3][2]);
    const glm::vec3 up = glm::vec3(glm::inverse(orientationMatrix) * glm::vec4(0, 1, 0, 1));
    const glm::mat4 viewMatrix = orientationMatrix * translationMatrix;
    const glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

    const std::vector<Mesh*>& meshComponents = meshes.GetAll();

    { PROFILE("Render z-pass meshes");
    { GPUPROFILE("Render z-pass meshes", Video::Query::Type::TIME_ELAPSED);
    { GPUPROFILE("Render z-pass meshes", Video::Query::Type::SAMPLES_PASSED);
        for (Mesh* mesh : meshComponents) {
            if (mesh->IsKilled() || !mesh->entity->enabled)
                continue;

            if (mesh->geometry != nullptr && mesh->geometry->GetType() == Video::Geometry::Geometry3D::STATIC) {
                Entity* entity = mesh->entity; 
                // If entity does not have material, it won't be rendered.
                if (entity->GetComponent<Material>() != nullptr) {
                    renderer->DepthRenderStaticMesh(mesh->geometry, viewMatrix, projectionMatrix, entity->GetModelMatrix());
                }
            }
        }
    }
    }
    }

    // Render static meshes.
    { PROFILE("Render static meshes");
    { GPUPROFILE("Render static meshes", Video::Query::Type::TIME_ELAPSED);
    { GPUPROFILE("Render static meshes", Video::Query::Type::SAMPLES_PASSED);
        renderer->PrepareStaticMeshRendering(viewMatrix, projectionMatrix);
        for (Mesh* mesh : meshComponents) {
            if (mesh->IsKilled() || !mesh->entity->enabled)
                continue;

            if (mesh->geometry != nullptr && mesh->geometry->GetType() == Video::Geometry::Geometry3D::STATIC) {
                Entity* entity = mesh->entity;
                Material* material = entity->GetComponent<Material>();
                if (material != nullptr) {
                    renderer->RenderStaticMesh(mesh->geometry, material->albedo->GetTexture(), material->normal->GetTexture(), material->metallic->GetTexture(), material->roughness->GetTexture(), entity->GetModelMatrix(), mesh->GetSelected());
                }
            }
        }
    }
    }
    }

    /// @todo Render skinned meshes.
    
    // Light the world.
    { PROFILE("Light the world");
    { GPUPROFILE("Light the world", Video::Query::Type::TIME_ELAPSED);
    { //GPUPROFILE("Light the world", Video::Query::Type::SAMPLES_PASSED);
        LightWorld(world, viewMatrix, projectionMatrix, viewProjectionMatrix, renderSurface);
    }
    }
    }

    /*
    // Anti-aliasing.
    if (Hymn().filterSettings.fxaa) {
        { PROFILE("Anti-aliasing(FXAA)");
        { GPUPROFILE("Anti-aliasing(FXAA)", Video::Query::Type::TIME_ELAPSED);
        { GPUPROFILE("Anti-aliasing(FXAA)", Video::Query::Type::SAMPLES_PASSED);
            renderer->AntiAlias(renderSurface);
        }
        }
        }
    }
    
    
    // Fog.
    if (Hymn().filterSettings.fog) {
        { PROFILE("Fog");
        { GPUPROFILE("Fog", Video::Query::Type::TIME_ELAPSED);
        { GPUPROFILE("Fog", Video::Query::Type::SAMPLES_PASSED);
            renderer->RenderFog(renderSurface, projectionMatrix, Hymn().filterSettings.fogDensity, Hymn().filterSettings.fogColor);
        }
        }
        }
    }
    
    // Render particles.
    { PROFILE("Render particles");
    { GPUPROFILE("Render particles", Video::Query::Type::TIME_ELAPSED);
    { GPUPROFILE("Render particles", Video::Query::Type::SAMPLES_PASSED);
        Managers().particleManager->UpdateBuffer(world);
        Managers().particleManager->Render(world, position, up, viewProjectionMatrix);
    }
    }
    }
    
    
    // Glow.
    if (Hymn().filterSettings.glow) {
        { PROFILE("Glow");
        { GPUPROFILE("Glow", Video::Query::Type::TIME_ELAPSED);
        { GPUPROFILE("Glow", Video::Query::Type::SAMPLES_PASSED);
            renderer->ApplyGlow(renderSurface, Hymn().filterSettings.glowBlurAmount);
        }
        }
        }
    }
    
    // Color.
    if (Hymn().filterSettings.color) {
        { PROFILE("Color");
        { GPUPROFILE("Color", Video::Query::Type::TIME_ELAPSED);
        { GPUPROFILE("Color", Video::Query::Type::SAMPLES_PASSED);
            renderer->ApplyColorFilter(renderSurface, Hymn().filterSettings.colorColor);
        }
        }
        }
    }
    */

    renderSurface->GetPostProcessingFrameBuffer()->Unbind();
}

Component::Animation* RenderManager::CreateAnimation() {
    return animations.Create();
}

Component::Animation* RenderManager::CreateAnimation(const Json::Value& node) {
    Component::Animation* animation = animations.Create();
    
    // Load values from Json node.
    std::string name = node.get("riggedModel", "").asString();
    /// @todo Fix animation.
    /*for (Geometry::Model* model : Hymn().models) {
        if (model->name == name)
            riggedModel = model;
    }*/
    
    return animation;
}

const std::vector<Component::Animation*>& RenderManager::GetAnimations() const {
    return animations.GetAll();
}

Component::DirectionalLight* RenderManager::CreateDirectionalLight() {
    return directionalLights.Create();
}

Component::DirectionalLight* RenderManager::CreateDirectionalLight(const Json::Value& node) {
    Component::DirectionalLight* directionalLight = directionalLights.Create();
    
    // Load values from Json node.
    directionalLight->color = Json::LoadVec3(node["color"]);
    directionalLight->ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
    
    return directionalLight;
}

const std::vector<Component::DirectionalLight*>& RenderManager::GetDirectionalLights() const {
    return directionalLights.GetAll();
}

Component::Lens* RenderManager::CreateLens() {
    return lenses.Create();
}

Component::Lens* RenderManager::CreateLens(const Json::Value& node) {
    Component::Lens* lens = lenses.Create();
    
    // Load values from Json node.
    lens->fieldOfView = node.get("fieldOfView", 45.f).asFloat();
    lens->zNear = node.get("zNear", 0.5f).asFloat();
    lens->zFar = node.get("zFar", 100.f).asFloat();
    
    return lens;
}

const std::vector<Component::Lens*>& RenderManager::GetLenses() const {
    return lenses.GetAll();
}

Component::Material* RenderManager::CreateMaterial() {
    return materials.Create();
}

Component::Material* RenderManager::CreateMaterial(const Json::Value& node) {
    Component::Material* material = materials.Create();
    
    // Load values from Json node.
    LoadTexture(material->albedo, node.get("albedo", "").asString());
    LoadTexture(material->normal, node.get("normal", "").asString());
    LoadTexture(material->metallic, node.get("metallic", "").asString());
    LoadTexture(material->roughness, node.get("roughness", "").asString());
    
    return material;
}

const std::vector<Component::Material*>& RenderManager::GetMaterials() const {
    return materials.GetAll();
}

Component::Mesh* RenderManager::CreateMesh() {
    return meshes.Create();
}

Component::Mesh* RenderManager::CreateMesh(const Json::Value& node) {
    Component::Mesh* mesh = meshes.Create();
    
    // Load values from Json node.
    std::string meshName = node.get("model", "").asString();
    mesh->geometry = Managers().resourceManager->CreateModel(meshName);
    
    return mesh;
}

const std::vector<Component::Mesh*>& RenderManager::GetMeshes() const {
    return meshes.GetAll();
}

Component::PointLight* RenderManager::CreatePointLight() {
    return pointLights.Create();
}

Component::PointLight* RenderManager::CreatePointLight(const Json::Value& node) {
    Component::PointLight* pointLight = pointLights.Create();
    
    // Load values from Json node.
    pointLight->color = Json::LoadVec3(node["color"]);
    pointLight->ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
    pointLight->attenuation = node.get("attenuation", 1.f).asFloat();
    pointLight->intensity = node.get("intensity", 1.f).asFloat();
    
    return pointLight;
}

const std::vector<Component::PointLight*>& RenderManager::GetPointLights() const {
    return pointLights.GetAll();
}

Component::SpotLight* RenderManager::CreateSpotLight() {
    return spotLights.Create();
}

Component::SpotLight* RenderManager::CreateSpotLight(const Json::Value& node) {
    Component::SpotLight* spotLight = spotLights.Create();
    
    // Load values from Json node.
    spotLight->color = Json::LoadVec3(node["color"]);
    spotLight->ambientCoefficient = node.get("ambientCoefficient", 0.5f).asFloat();
    spotLight->attenuation = node.get("attenuation", 1.f).asFloat();
    spotLight->intensity = node.get("intensity", 1.f).asFloat();
    spotLight->coneAngle = node.get("coneAngle", 15.f).asFloat();
    
    return spotLight;
}

const std::vector<Component::SpotLight*>& RenderManager::GetSpotLights() const {
    return spotLights.GetAll();
}

void RenderManager::ClearKilledComponents() {
    animations.ClearKilled();
    directionalLights.ClearKilled();
    lenses.ClearKilled();
    materials.ClearKilled();
    meshes.ClearKilled();
    pointLights.ClearKilled();
    spotLights.ClearKilled();
}

void RenderManager::LightWorld(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& viewProjectionMatrix, Video::RenderSurface* renderSurface) {

    float cutOff;
    Video::AxisAlignedBoundingBox aabb(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));
    
    // Add all directional lights.
    for (Component::DirectionalLight* directionalLight : directionalLights.GetAll()) {
        if (directionalLight->IsKilled() || !directionalLight->entity->enabled)
            continue;
        
        Entity* lightEntity = directionalLight->entity;
        glm::vec4 direction(glm::vec4(lightEntity->GetDirection(), 0.f));
        Video::Light light;
        light.position = viewMatrix * -direction;
        light.intensities = directionalLight->color;
        light.attenuation = 1.f;
        light.ambientCoefficient = directionalLight->ambientCoefficient;
        light.coneAngle = 0.f;
        light.direction = glm::vec3(0.f, 0.f, 0.f);
        renderer->AddLight(light);
    }
    
    // Add all spot lights.
    for (Component::SpotLight* spotLight : spotLights.GetAll()) {
        if (spotLight->IsKilled() || !spotLight->entity->enabled)
            continue;
        
        Entity* lightEntity = spotLight->entity;
        glm::vec4 direction(viewMatrix * glm::vec4(lightEntity->GetDirection(), 0.f));
        glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
        Video::Light light;
        light.position = viewMatrix * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0));
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
    for (Component::PointLight* pointLight : pointLights.GetAll()) {
        if (pointLight->IsKilled() || !pointLight->entity->enabled)
            continue;
        
        Entity* lightEntity = pointLight->entity;
        float scale = sqrt((1.f / cutOff - 1.f) / pointLight->attenuation);
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->GetWorldPosition()) * glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f) * scale);
        
        Video::Frustum frustum(viewProjectionMatrix * modelMat);
        if (frustum.Collide(aabb)) {
            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            Video::Light light;
            light.position = viewMatrix * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0));
            light.intensities = pointLight->color * pointLight->intensity;
            light.attenuation = pointLight->attenuation;
            light.ambientCoefficient = pointLight->ambientCoefficient;
            light.coneAngle = 180.f;
            light.direction = glm::vec3(1.f, 0.f, 0.f);
            renderer->AddLight(light);
        }
    }
    
    // Render lights.
    renderer->Light(glm::inverse(projectionMatrix), renderSurface);
}

void RenderManager::LoadTexture(TextureAsset*& texture, const std::string& name) {
    if (!name.empty())
        texture = Managers().resourceManager->CreateTextureAsset(name);
}
