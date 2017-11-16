#pragma once

#include <glm/glm.hpp>
#include "../Entity/ComponentContainer.hpp"
#include <string>
#include "../linking.hpp"

namespace Video {
    class Renderer;
    class RenderSurface;
    class TexturePNG;
    class ShadowPass;
} // namespace Video
class World;
class Entity;
namespace Component {
    class AnimationController;
    class DirectionalLight;
    class Lens;
    class Material;
    class Mesh;
    class PointLight;
    class SpotLight;
} // namespace Component
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
         * @param soundSources Whether to show sound sources.
         * @param particleEmitters Whether to show particle emitters.
         * @param lightSources Whether to show light sources.
         * @param cameras Whether to show cameras.
         * @param physics Whether to show physics volumes.
         * @param camera Camera through which to render (or first camera in the world if nullptr).
         * @param lighting Whether to light the scene (otherwise full ambient is used).
         */
        ENGINE_API void Render(World& world, bool soundSources = true, bool particleEmitters = true, bool lightSources = true, bool cameras = true, bool physics = true, Entity* camera = nullptr, bool lighting = true);
        
        /// Update all the animations in the scene.
        /**
         * @param deltaTime Time between frames.
         */
        ENGINE_API void UpdateAnimations(float deltaTime);

        /// Updates the buffers to fit the current screen size.
        ENGINE_API void UpdateBufferSize();

        /// Create animation component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::AnimationController* CreateAnimation();
  
        /// Create animation component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::AnimationController* CreateAnimation(const Json::Value& node);

        /// Get all animation controller components.
        /**
         * @return All animation controller components.
         */
        ENGINE_API const std::vector<Component::AnimationController*>& GetAnimations() const;

        /// Create directional light component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::DirectionalLight* CreateDirectionalLight();

        /// Create directional light component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::DirectionalLight* CreateDirectionalLight(const Json::Value& node);

        /// Get all directional light components.
        /**
         * @return All directional light components.
         */
        ENGINE_API const std::vector<Component::DirectionalLight*>& GetDirectionalLights() const;

        /// Create lens component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::Lens* CreateLens();

        /// Create lens component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::Lens* CreateLens(const Json::Value& node);

        /// Get all lens components.
        /**
         * @return All lens components.
         */
        ENGINE_API const std::vector<Component::Lens*>& GetLenses() const;

        /// Create material component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::Material* CreateMaterial();

        /// Create material component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::Material* CreateMaterial(const Json::Value& node);

        /// Get all material components.
        /**
         * @return All material components.
         */
        ENGINE_API const std::vector<Component::Material*>& GetMaterials() const;

        /// Create mesh component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::Mesh* CreateMesh();

        /// Create mesh component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::Mesh* CreateMesh(const Json::Value& node);

        /// Get all mesh components.
        /**
         * @return All mesh components.
         */
        ENGINE_API const std::vector<Component::Mesh*>& GetMeshes() const;

        /// Create point light component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::PointLight* CreatePointLight();

        /// Create point light component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::PointLight* CreatePointLight(const Json::Value& node);

        /// Get all point light components.
        /**
         * @return All point light components.
         */
        ENGINE_API const std::vector<Component::PointLight*>& GetPointLights() const;

        /// Create spot light component.
        /**
         * @return The created component.
         */
        ENGINE_API Component::SpotLight* CreateSpotLight();

        /// Create spot light component.
        /**
         * @param node Json node to load the component from.
         * @return The created component.
         */
        ENGINE_API Component::SpotLight* CreateSpotLight(const Json::Value& node);

        /// Get all spot light components.
        /**
         * @return All spot light components.
         */
        ENGINE_API const std::vector<Component::SpotLight*>& GetSpotLights() const;

        /// Remove all killed components.
        ENGINE_API void ClearKilledComponents();

        /// Set gamma correction.
        /**
         * @param gamma Gamma to apply.
         */
        ENGINE_API void SetGamma(float gamma);

        /// Get gamma correction.
        /**
         * @return gamma Gamma value.
         */
        ENGINE_API float GetGamma() const;


        /// Set whether fog is applied.
        /**
         * @param fogApply Whether to apply fog.
         */
        ENGINE_API void SetFogApply(bool fogApply);

        /// Get whether fog is applied.
        /**
         * @return Whether fog is applied.
         */
        ENGINE_API bool GetFogApply() const;


        /// Set fog density.
        /**
         * @param fogDensity Density of the fog.
         */
        ENGINE_API void SetFogDensity(float fogDensity);

        /// Get fog density.
        /**
         * @return Density of the fog
         */
        ENGINE_API float GetFogDensity() const;


        /// Set fog color.
        /**
         * @param fogColor Color of the fog.
         */
        ENGINE_API void SetFogColor(const glm::vec3& fogColor);

        /// Get fog color.
        /**
         * @return Color of the fog.
         */
        ENGINE_API glm::vec3 GetFogColor() const;


        /// Set whether color filter is applied.
        /**
         * @param colorFilterApply Whether to apply color filter.
         */
        ENGINE_API void SetColorFilterApply(bool colorFilterApply);

        /// Get whether color filter is applied.
        /**
         * @return Whether color filter is applied.
         */
        ENGINE_API bool GetColorFilterApply() const;


        /// Set color filer color.
        /**
         * @param colorFilterColor Color of the color filter.
         */
        ENGINE_API void SetColorFilterColor(const glm::vec3& colorFilterColor);

        /// Get color filer color.
        /**
         * @return Color of the color filter.
         */
        ENGINE_API glm::vec3 GetColorFilterColor() const;


        /// Set whether dithering is applied.
        /**
         * @param ditherApply Whether to apply dithering.
         */
        ENGINE_API void SetDitherApply(bool ditherApply);

        /// Get whether dithering is applied.
        /**
         * @return Whether dithering is applied.
         */
        ENGINE_API bool GetDitherApply() const;

    private:
        RenderManager();
        ~RenderManager();
        RenderManager(RenderManager const&) = delete;
        void operator=(RenderManager const&) = delete;

        void RenderWorldEntities(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface, bool lighting);

        void RenderEditorEntities(World& world, bool soundSources, bool particleEmitters, bool lightSources, bool cameras, bool physics, const glm::vec3& position, const glm::vec3& up, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface);

        void LightWorld(World& world, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& viewProjectionMatrix);
        void LightAmbient();

        void LoadTexture(TextureAsset*& texture, const std::string& name);

        Video::Renderer* renderer;

        Video::ShadowPass* shadowPass;

        Video::RenderSurface* mainWindowRenderSurface;
        Video::RenderSurface* hmdRenderSurface;

        // Editor entity textures.
        Video::TexturePNG* particleEmitterTexture;
        Video::TexturePNG* lightTexture;
        Video::TexturePNG* soundSourceTexture;
        Video::TexturePNG* cameraTexture;

        // Components.
        ComponentContainer<Component::AnimationController> animationControllers;
        ComponentContainer<Component::DirectionalLight> directionalLights;
        ComponentContainer<Component::Lens> lenses;
        ComponentContainer<Component::Material> materials;
        ComponentContainer<Component::Mesh> meshes;
        ComponentContainer<Component::PointLight> pointLights;
        ComponentContainer<Component::SpotLight> spotLights;
};
