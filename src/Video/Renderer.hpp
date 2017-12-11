#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Lighting/Light.hpp"
#include "linking.hpp"

namespace Video {
    class StaticRenderProgram;
    class SkinRenderProgram;
    class Texture2D;
    class PostProcessing;
    class FXAAFilter;
    class ShaderProgram;
    class RenderSurface;
    class StorageBuffer;
    namespace Geometry {
        class Geometry3D;
        class Rectangle;
    } // namespace Geometry

    /// Handles rendering using OpenGL.
    class Renderer {
        public:
            /// Create new renderer.
            VIDEO_API Renderer();

            /// Destructor.
            VIDEO_API ~Renderer();

            /// Start rendering the frame.
            /**
             * @param renderSurface %RenderSurface to render to.
             */
            VIDEO_API void StartRendering(RenderSurface* renderSurface);

            /// Prepare for shadow rendering static meshes.
            /**
             * @param lightView The lights view matrix
             * @param lightProjection the Lights projection matrix
             * @param shadowId the texture ID for the shadowMap
             * @param shadowWidth size x of shadowMap
             * @param shadowHeight size y of shadowMap
             * @param depthFbo framebuffer object for shadowMap
             */
            VIDEO_API void PrepareStaticShadowRendering(const glm::mat4 lightView, glm::mat4 lightProjection, int shadowId, int shadowWidth, int shadowHeight, int dephtFbo);

            /// Render a static shadow mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             */
            VIDEO_API void ShadowRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);
        
            /// Prepare for depth rendering static meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            VIDEO_API void PrepareStaticMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render a static mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             */
            VIDEO_API void DepthRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);

            /// Prepare for rendering static meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            VIDEO_API void PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render a static mesh.
            /**
             * @param geometry The geometry to render.
             * @param albedo Albedo texture.
             * @param normal Normal map.
             * @param metallic Metallic map.
             * @param roughness Roughness texture.
             * @param modelMatrix Model matrix.
             */
            VIDEO_API void RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix);

            /// Prepare for shadow rendering skin meshes.
            /**
             * @param lightView The lights view matrix
             * @param lightProjection the Lights projection matrix
             * @param shadowId the texture ID for the shadowMap
             * @param shadowWidth size x of shadowMap
             * @param shadowHeight size y of shadowMap
             * @param depthFbo framebuffer object for shadowMap
            */
            VIDEO_API void PrepareSkinShadowRendering(const glm::mat4 lightView, glm::mat4 lightProjection, int shadowId, int shadowWidth, int shadowHeight, int dephtFbo);

            /// Prepare for depth rendering skin meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            VIDEO_API void PrepareSkinMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render a skin mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             * @param bones Bones array.
             */
            VIDEO_API void DepthRenderSkinMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones);

            /// Prepare for rendering skin meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            VIDEO_API void PrepareSkinMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render a skin shadow mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             * @param bones Bones array.
             */
            VIDEO_API void ShadowRenderSkinMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones);

            /// Render a skin mesh.
            /**
             * @param geometry The geometry to render.
             * @param albedo Albedo texture.
             * @param normal Normal map.
             * @param metallic Metallic map.
             * @param roughness Roughness texture.
             * @param modelMatrix Model matrix.
             * @param bones Bones array.
             */
            VIDEO_API void RenderSkinMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix, const std::vector<glm::mat4>& bones);

            /// Update light buffer.
            /**
             * @param lights Vector of lights to push to the light buffer.
             */
            VIDEO_API void SetLights(const std::vector<Video::Light>& lights);

            /// Anti-alias using FXAA.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             */
            VIDEO_API void AntiAlias(RenderSurface* renderSurface);

            /// Render fog.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param projectionMatrix The camera's projection matrix.
             * @param density The density of the fog.
             * @param color Color.
             */
            VIDEO_API void RenderFog(RenderSurface* renderSurface, const glm::mat4& projectionMatrix, float density, const glm::vec3& color);

            /// Apply glow effect.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param blurAmount How many times to blur the glow buffer.
             */
            VIDEO_API void ApplyGlow(RenderSurface* renderSurface, int blurAmount);

            /// Apply a color filter.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param color Color.
             */
            VIDEO_API void ApplyColorFilter(RenderSurface* renderSurface, const glm::vec3& color);

            /// Display the rendered results to back buffer.
            /**
             * @param renderSurface RenderSurface to present to back buffer.
             * @param targetSize Size of the back buffer in pixels.
             */
            VIDEO_API void Present(RenderSurface* renderSurface, const glm::vec2& targetSize);

            /// Begin rendering icons.
            /**
             * \note 
             * Needs to be called before RenderIcon.
             * @param viewProjectionMatrix The camera's view projection matrix.
             * @param cameraPosition The camera's position.
             * @param cameraUp The camera's up vector.
             */
            VIDEO_API void PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp);

            /// Render a billboarded icon.
            /**
             * \note 
             * PrepareRenderingIcons must be called before.
             * @param position World position to render at.
             * @param icon The icon to render.
             */
            VIDEO_API void RenderIcon(const glm::vec3& position, const Texture2D* icon);

            /// Stop rendering icons.
            /**
             * \note 
             * Should be called after all icons have been rendered.
             */
            VIDEO_API void StopRenderingIcons();

            /// Gamma correction.
            /**
             * @param gamma Gamma to apply.
             */
            VIDEO_API void SetGamma(float gamma);

            /// Gamma correction.
            /**
             * @return gamma Gamma value.
             */
            VIDEO_API float GetGamma() const;


            /// Set whether fog is applied.
            /**
             * @param fogApply Whether to apply fog.
             */
            VIDEO_API void SetFogApply(bool fogApply);

            /// Get whether fog is applied.
            /**
             * @return Whether fog is applied.
             */
            VIDEO_API bool GetFogApply() const;


            /// Set fog density.
            /**
             * @param fogDensity Density of the fog.
             */
            VIDEO_API void SetFogDensity(float fogDensity);

            /// get fog density.
            /**
             * @return Density of the fog
             */
            VIDEO_API float GetFogDensity() const;


            /// Set fog color.
            /**
             * @param fogColor Color of the fog.
             */
            VIDEO_API void SetFogColor(const glm::vec3& fogColor);

            /// Get fog color.
            /**
             * @return Color of the fog.
             */
            VIDEO_API glm::vec3 GetFogColor() const;


            /// Set whether color filter is applied.
            /**
             * @param colorFilterApply Whether to apply color filter.
             */
            VIDEO_API void SetColorFilterApply(bool colorFilterApply);

            /// Get whether color filter is applied.
            /**
             * @return Whether color filter is applied.
             */
            VIDEO_API bool GetColorFilterApply() const;


            /// Set color filer color.
            /**
             * @param colorFilterColor Color of the color filter.
             */
            VIDEO_API void SetColorFilterColor(const glm::vec3& colorFilterColor);

            /// Get color filer color.
            /**
             * @return Color of the color filter.
             */
            VIDEO_API glm::vec3 GetColorFilterColor() const;


            /// Set whether dithering is applied.
            /**
             * @param ditherApply Whether to apply dithering.
             */
            VIDEO_API void SetDitherApply(bool ditherApply);

            /// Get whether dithering is applied.
            /**
             * @return Whether dithering is applied.
             */
            VIDEO_API bool GetDitherApply() const;


            /// Set size of the current frame to be rendered in pixels.
            /**
             * @param frameSize Size of the frame in pixels.
             */
            VIDEO_API void SetFrameSize(const glm::vec2& frameSize);

            /// Get size of the current frame to be rendered in pixels.
            /**
             * @return Size of the frame in pixels.
             */
            VIDEO_API glm::vec2 GetFrameSize() const;
            
            /// Set whether blood is applied.
            /**
             * @param bloodApply Whether blood is applied.
             */
            VIDEO_API void SetBloodApply(bool bloodApply);


        private:
            Renderer(const Renderer& other) = delete;
            StaticRenderProgram* staticRenderProgram;
            SkinRenderProgram* skinRenderProgram;

            unsigned int lightCount;
            StorageBuffer* lightBuffer;

            PostProcessing* postProcessing;
            FXAAFilter* fxaaFilter;

            // Icon rendering.
            ShaderProgram* iconShaderProgram;
            GLuint vertexBuffer;
            GLuint vertexArray;
            const Texture2D* currentIcon = nullptr;
            
            // Uniform locations.
            GLuint viewProjectionLocation;
            GLuint cameraPositionLocation;
            GLuint cameraUpLocation;
            GLuint baseImageLocation;
            GLuint positionLocation;

            Geometry::Rectangle* rectangle;
    };
} // namespace Video
