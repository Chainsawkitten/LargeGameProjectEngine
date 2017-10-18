#pragma once

#include <glm/glm.hpp>
#include "../Entity/ComponentContainer.hpp"
#include <string>

namespace Video {
    class Renderer;
    class RenderSurface;
    class Texture2D;
}
class World;
class Entity;
namespace Component {
    class AnimationController;
    class Controller;
    class DirectionalLight;
    class Lens;
    class Material;
    class Mesh;
    class PointLight;
    class SpotLight;
}
namespace Json {
    class Value;
}
class TextureAsset;

/// Handles rendering the world.
class RenderManager {
    friend class Hub;
    
    public:
        /// Render world containing entities.
        /**
         * @param world Contains a bunch of entities.
         * @param camera Camera through which to render (or first camera in the world if nullptr).
         */
        void Render(World& world, Entity* camera = nullptr);
        
        /// Render editor entities.
        /**
         * @param world World to render.
         * @param camera Camera through which to render (or first camera in the world if nullptr).
         * @param soundSources Whether to show sound sources.
         * @param particleEmitters Whether to show particle emitters.
         * @param lightSources Whether to show light sources.
         * @param cameras Whether to show cameras.
         * @param physics Whether to show physics volumes.
         */
        void RenderEditorEntities(World& world, Entity* camera = nullptr, bool soundSources = true, bool particleEmitters = true, bool lightSources = true, bool cameras = true, bool physics = true);
        
        /// Updates the buffers to fit the current screen size.
        void UpdateBufferSize();

        /// Update all the animations in the scene.
        /**
         * @param deltaTime Time between frames.
         */
        void UpdateAnimations(float deltaTime);
        
        /// Create animation component.
        /**
         * @return The created component.
         */
        Component::AnimationController* CreateAnimation();
        
        /// Create animation component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::AnimationController* CreateAnimation(const Json::Value& node);
        
        /// Get all animation components.
        /**
         * @return All animation components.
         */
        const std::vector<Component::AnimationController*>& GetAnimations() const;
        
        /// Create directional light component.
        /**
         * @return The created component.
         */
        Component::DirectionalLight* CreateDirectionalLight();
        
        /// Create directional light component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::DirectionalLight* CreateDirectionalLight(const Json::Value& node);
        
        /// Get all directional light components.
        /**
         * @return All directional light components.
         */
        const std::vector<Component::DirectionalLight*>& GetDirectionalLights() const;
        
        /// Create lens component.
        /**
         * @return The created component.
         */
        Component::Lens* CreateLens();
        
        /// Create lens component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::Lens* CreateLens(const Json::Value& node);
        
        /// Get all lens components.
        /**
         * @return All lens components.
         */
        const std::vector<Component::Lens*>& GetLenses() const;
        
        /// Create material component.
        /**
         * @return The created component.
         */
        Component::Material* CreateMaterial();
        
        /// Create material component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::Material* CreateMaterial(const Json::Value& node);
        
        /// Get all material components.
        /**
         * @return All material components.
         */
        const std::vector<Component::Material*>& GetMaterials() const;
        
        /// Create mesh component.
        /**
         * @return The created component.
         */
        Component::Mesh* CreateMesh();
        
        /// Create mesh component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::Mesh* CreateMesh(const Json::Value& node);
        
        /// Get all mesh components.
        /**
         * @return All mesh components.
         */
        const std::vector<Component::Mesh*>& GetMeshes() const;
        
        /// Create point light component.
        /**
         * @return The created component.
         */
        Component::PointLight* CreatePointLight();
        
        /// Create point light component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::PointLight* CreatePointLight(const Json::Value& node);
        
        /// Get all point light components.
        /**
         * @return All point light components.
         */
        const std::vector<Component::PointLight*>& GetPointLights() const;
        
        /// Create spot light component.
        /**
         * @return The created component.
         */
        Component::SpotLight* CreateSpotLight();
        
        /// Create spot light component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        Component::SpotLight* CreateSpotLight(const Json::Value& node);
        
        /// Get all spot light components.
        /**
         * @return All spot light components.
         */
        const std::vector<Component::SpotLight*>& GetSpotLights() const;

        /// Create vr controller component
        /**
         * @return The created component.
         */
        Component::Controller* CreateController();

        /// Create vr controller component
        /**
         * @param node Json node to load the component from
         * @return The created component.
         */
        Component::Controller* CreateController(const Json::Value& node);

        /// Get all vr controller components
        /**
         * @return All vr controller components
         */
        const std::vector<Component::Controller*>& GetControllers() const;
        
        /// Remove all killed components.
        void ClearKilledComponents();
        
    private:
        RenderManager();
        ~RenderManager();
        RenderManager(RenderManager const&) = delete;
        void operator=(RenderManager const&) = delete;

        void Render(World& world, const glm::mat4& translationMatrix, const glm::mat4& orientationMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface);
        
        void LightWorld(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& viewProjectionMatrix);

        void LoadTexture(TextureAsset*& texture, const std::string& name);

        Video::Renderer* renderer;

        Video::RenderSurface* mainWindowRenderSurface;
        Video::RenderSurface* hmdRenderSurface;

        // Editor entity textures.
        Video::Texture2D* particleEmitterTexture;
        Video::Texture2D* lightTexture;
        Video::Texture2D* soundSourceTexture;
        Video::Texture2D* cameraTexture;
        
        // Components.
        ComponentContainer<Component::AnimationController> animationControllers;
        ComponentContainer<Component::Controller> controllers;
        ComponentContainer<Component::DirectionalLight> directionalLights;
        ComponentContainer<Component::Lens> lenses;
        ComponentContainer<Component::Material> materials;
        ComponentContainer<Component::Mesh> meshes;
        ComponentContainer<Component::PointLight> pointLights;
        ComponentContainer<Component::SpotLight> spotLights;
};
