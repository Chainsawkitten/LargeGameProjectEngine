#include "RenderManager.hpp"

#include <GL/glew.h>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "ParticleManager.hpp"
#include "DebugDrawingManager.hpp"
#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"
#include "Skinning.vert.hpp"
#include "EditorEntity.vert.hpp"
#include "EditorEntity.geom.hpp"
#include "EditorEntity.frag.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../RenderProgram/SkinRenderProgram.hpp"
#include "../RenderProgram/StaticRenderProgram.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Transform.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Geometry/Geometry3D.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Lighting/DeferredLighting.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Physics/Frustum.hpp"
#include "../MainWindow.hpp"
#include "../RenderTarget.hpp"
#include "../PostProcessing/PostProcessing.hpp"
#include "../PostProcessing/FXAAFilter.hpp"
#include "../PostProcessing/GammaCorrectionFilter.hpp"
#include "../PostProcessing/GlowFilter.hpp"
#include "../PostProcessing/GlowBlurFilter.hpp"

using namespace Component;

RenderManager::RenderManager() {
    defaultVertexShader = Managers().resourceManager->CreateShader(DEFAULT3D_VERT, DEFAULT3D_VERT_LENGTH, GL_VERTEX_SHADER);
    skinningVertexShader = Managers().resourceManager->CreateShader(SKINNING_VERT, SKINNING_VERT_LENGTH, GL_VERTEX_SHADER);
    defaultFragmentShader = Managers().resourceManager->CreateShader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    staticShaderProgram = Managers().resourceManager->CreateShaderProgram({ defaultVertexShader, defaultFragmentShader });
    skinShaderProgram = Managers().resourceManager->CreateShaderProgram({ skinningVertexShader, defaultFragmentShader });
    staticRenderProgram = new StaticRenderProgram(staticShaderProgram);
    skinRenderProgram = new SkinRenderProgram(skinShaderProgram);
    
    editorEntityVertexShader = Managers().resourceManager->CreateShader(EDITORENTITY_VERT, EDITORENTITY_VERT_LENGTH, GL_VERTEX_SHADER);
    editorEntityGeometryShader = Managers().resourceManager->CreateShader(EDITORENTITY_GEOM, EDITORENTITY_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    editorEntityFragmentShader = Managers().resourceManager->CreateShader(EDITORENTITY_FRAG, EDITORENTITY_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    editorEntityShaderProgram = Managers().resourceManager->CreateShaderProgram({ editorEntityVertexShader, editorEntityGeometryShader, editorEntityFragmentShader });

    deferredLighting = new DeferredLighting();
    
    postProcessing = new PostProcessing();
    fxaaFilter = new FXAAFilter();
    gammaCorrectionFilter = new GammaCorrectionFilter();
    glowFilter = new GlowFilter();
    glowBlurFilter = new GlowBlurFilter();
    
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeShader(defaultVertexShader);
    Managers().resourceManager->FreeShader(skinningVertexShader);
    Managers().resourceManager->FreeShader(defaultFragmentShader);
    Managers().resourceManager->FreeShaderProgram(staticShaderProgram);
    Managers().resourceManager->FreeShaderProgram(skinShaderProgram);
    delete staticRenderProgram;
    delete skinRenderProgram;
    
    Managers().resourceManager->FreeShader(editorEntityVertexShader);
    Managers().resourceManager->FreeShader(editorEntityGeometryShader);
    Managers().resourceManager->FreeShader(editorEntityFragmentShader);
    Managers().resourceManager->FreeShaderProgram(editorEntityShaderProgram);

    delete deferredLighting;
    
    delete postProcessing;
    delete fxaaFilter;
    delete gammaCorrectionFilter;
    delete glowFilter;
    delete glowBlurFilter;
}

void RenderManager::Render(Scene& scene) {
    // Find camera entity.
    Entity* camera = nullptr;
    std::vector<Lens*> lenses = scene.GetComponents<Lens>();
    for (Lens* lens : lenses) {
        if (lens->entity->GetComponent<Transform>() != nullptr)
            camera = lens->entity;
    };
    
    // Render from camera.
    if (camera != nullptr) {
        deferredLighting->SetTarget();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec2 screenSize(MainWindow::GetInstance()->GetSize());
        glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
        
        std::vector<Mesh*> meshes = scene.GetComponents<Mesh>();
        // Static render program.
        staticRenderProgram->PreRender(camera, screenSize);
        for (Mesh* mesh : meshes)
            if (mesh->geometry->GetType() == Geometry::Geometry3D::STATIC)
                staticRenderProgram->Render(mesh);
        staticRenderProgram->PostRender();

        // Skin render program.
        skinRenderProgram->PreRender(camera, screenSize);
        for (Mesh* mesh : meshes)
            if (mesh->geometry->GetType() == Geometry::Geometry3D::SKIN)
                skinRenderProgram->Render(mesh);
        skinRenderProgram->PostRender();
        
        // Light the scene.
        postProcessing->GetRenderTarget()->SetTarget();
        deferredLighting->Render(scene, camera);
        
        // Anti-aliasing.
        fxaaFilter->SetScreenSize(screenSize);
        postProcessing->ApplyFilter(fxaaFilter);
        
        // Render particles.
        Managers().particleManager->UpdateBuffer(scene);
        Managers().particleManager->Render(scene, camera);
        
        // Glow.
        glowBlurFilter->SetScreenSize(screenSize);
        int blurAmount = 1;
        for (int i = 0; i < blurAmount; ++i) {
            glowBlurFilter->SetHorizontal(true);
            postProcessing->ApplyFilter(glowBlurFilter);
            glowBlurFilter->SetHorizontal(false);
            postProcessing->ApplyFilter(glowBlurFilter);
        }
        postProcessing->ApplyFilter(glowFilter);
        
        // Gamma correction.
        postProcessing->ApplyFilter(gammaCorrectionFilter);
        
        // Render to back buffer.
        postProcessing->Render(true);
    }
}

void RenderManager::RenderEditorEntities(Scene& scene) {
    // Find camera entity.
    Entity* camera = nullptr;
    std::vector<Lens*> lenses = scene.GetComponents<Lens>();
    for (Lens* lens : lenses) {
        if (lens->entity->GetComponent<Transform>() != nullptr)
            camera = lens->entity;
    };
    
    // Render from camera.
    if (camera != nullptr) {
        /// @todo Render sound sources
        
        /// @todo Render particle emitter
        for (Component::ParticleEmitter* emitter : scene.GetComponents<Component::ParticleEmitter>()) {
            Entity* entity = emitter->entity;
            Component::Transform* transform = entity->GetComponent<Component::Transform>();
            if (transform != nullptr) {
                Managers().debugDrawingManager->AddPoint(transform->position, glm::vec3(1.f, 1.f, 1.f), 2.f);
            }
        }
        
        /// @todo Render light sources
    }
}
