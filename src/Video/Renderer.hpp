#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace Video {
    struct Light;
    class Lighting;
    class StaticRenderProgram;
    class SkinRenderProgram;
    class Texture2D;
    class PostProcessing;
    class ColorFilter;
    class FogFilter;
    class FXAAFilter;
    class GammaCorrectionFilter;
    class GlowBlurFilter;
    class GlowFilter;
    class ShaderProgram;
    namespace Geometry {
        class Geometry3D;
        class Rectangle;
    }
    
    /// Handles rendering using OpenGL.
    class Renderer {
        public:
            /// Create new renderer.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            Renderer(const glm::vec2& screenSize);
            
            /// Destructor.
            ~Renderer();
            
            /// Set screen size.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void SetScreenSize(const glm::vec2& screenSize);
            
            /// Clear the previous frame's data.
            void Clear();
            
            /// Start rendering the frame.
            void StartRendering();
            
            /// Prepare for rendering static meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
            
            /// Render a static mesh.
            /**
             * @param geometry The geometry to render.
             * @param diffuseTexture Diffuse texture.
             * @param normalTexture Normal map.
             * @param specularTexture Specular map.
             * @param glowTexture Glow texture.
             * @param modelMatrix Model matrix.
             */
            void RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* diffuseTexture, const Texture2D* normalTexture, const Texture2D* specularTexture, const Texture2D* glowTexture, const glm::mat4 modelMatrix);
            
            /// Prepare for rendering skinned meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PrepareSkinnedMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
            
            /// Render a skinned mesh.
            /**
             * @param geometry The geometry to render.
             * @param diffuseTexture Diffuse texture.
             * @param normalTexture Normal map.
             * @param specularTexture Specular map.
             * @param glowTexture Glow texture.
             * @param modelMatrix Model matrix.
             * @param bones Transformations of skeleton.
             * @param bonesIT Inverse transpose transformations of skeleton.
             */
            void RenderSkinnedMesh(const Video::Geometry::Geometry3D* geometry, const Video::Texture2D* diffuseTexture, const Video::Texture2D* normalTexture, const Video::Texture2D* specularTexture, const Video::Texture2D* glowTexture, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones, const std::vector<glm::mat3>& bonesIT);
            
            /// Add a light to the scene.
            void AddLight(const Video::Light& light);
            
            /// Light the scene with the added lights.
            /**
             * @param inverseProjectionMatrix The camera's inverse projection matrix.
             */
            void Light(const glm::mat4& inverseProjectionMatrix);
            
            /// Anti-alias using FXAA.
            void AntiAlias();
            
            /// Render fog.
            /**
             * @param projectionMatrix The camera's projection matrix.
             * @param density The density of the fog.
             * @param color Color.
             */
            void RenderFog(const glm::mat4& projectionMatrix, float density, const glm::vec3& color);
            
            /// Apply glow effect.
            /**
             * @param blurAmount How many times to blur the glow buffer.
             */
            void ApplyGlow(int blurAmount);
            
            /// Apply a color filter.
            /**
             * @param color Color.
             */
            void ApplyColorFilter(const glm::vec3& color);
            
            /// Perform gamma correction.
            void GammaCorrect();
            
            /// Display the rendered results.
            /**
             * @param dither Whether to use dithering.
             */
            void DisplayResults(bool dither);
            
            /// Begin rendering icons.
            /**
             * Needs to be called before RenderIcon.
             * @param viewProjectionMatrix The camera's view projection matrix.
             * @param cameraPosition The camera's position.
             * @param cameraUp The camera's up vector.
             */
            void PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
            
            /// Render a billboarded icon.
            /**
             * PrepareRenderingIcons must be called before.
             * @param position World position to render at.
             * @param icon The icon to render.
             */
            void RenderIcon(const glm::vec3& position, const Texture2D* icon);
            
            /// Stop rendering icons.
            /**
             * Should be called after all icons have been rendered.
             */
            void StopRenderingIcons();
            
        private:
            glm::vec2 screenSize;
            Lighting* lighting;
            StaticRenderProgram* staticRenderProgram;
            SkinRenderProgram* skinRenderProgram;
            
            PostProcessing* postProcessing;
            ColorFilter* colorFilter;
            FogFilter* fogFilter;
            FXAAFilter* fxaaFilter;
            GammaCorrectionFilter* gammaCorrectionFilter;
            GlowFilter* glowFilter;
            GlowBlurFilter* glowBlurFilter;
            
            // Icon rendering.
            ShaderProgram* iconShaderProgram;
            GLuint vertexBuffer;
            GLuint vertexArray;
            const Texture2D* currentIcon = nullptr;
            
            Geometry::Rectangle* rectangle;
    };
}
