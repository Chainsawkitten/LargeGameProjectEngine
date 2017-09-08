#pragma once

#include <map>
#include <GL/glew.h>

namespace Video {
    class Shader;
    class ShaderProgram;
    class Texture2D;
    namespace Geometry {
        class Rectangle;
    }
}
namespace Geometry {
    class Cube;
    class Model;
}
namespace Audio {
    class SoundBuffer;
}

/// Handles all resources.
class ResourceManager {
    friend class Hub;
    
    public:
        /// Create a shader if it doesn't already exist.
        /**
         * @param source GLSL code for the shader.
         * @param sourceLength Length of the GLSL source code.
         * @param shaderType %Shader type. One of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
         * @return The shader instance
         */
        Video::Shader* CreateShader(const char* source, int sourceLength, GLenum shaderType);
        
        /// Free the reference to the shader.
        /**
         * Deletes the instance if no more references exist.
         * @param shader %Shader to dereference.
         */
        void FreeShader(Video::Shader* shader);
        
        /// Create shader program if it doesn't already exist.
        /**
         * Link together shaders into a shader program that can be run on the GPU.
         *
         * Sample:
         * \code{.cpp}
         * Shader* vertexShader = new Shader(vertexSource, vertexSourceLength, GL_VERTEX_SHADER);
         * Shader* geometryShader = new Shader(geometrySource, geometrySourceLength, GL_GEOMETRY_SHADER);
         * Shader* fragmentShader = new Shader(fragmentSource, fragmentSourceLength, GL_FRAGMENT_SHADER);
         * ShaderProgram* shaderProgram = new ResourceManager::GetInstance().CreateShaderProgram({ vertexShader, geometryShader, fragmentShader });
         * \endcode
         *
         * @param shaders List of shaders to link together.
         * @return The shader program instance
         */
        Video::ShaderProgram* CreateShaderProgram(std::initializer_list<const Video::Shader*> shaders);
        
        /// Free the reference to a shader program.
        /**
         * Deletes the instance if no more references exist.
         * @param shaderProgram %Shader program to dereference.
         */
        void FreeShaderProgram(Video::ShaderProgram* shaderProgram);
        
        /// Create a rectangle for rendering if it doesn't already exist.
        /**
         * @return The rectangle instance
         */
        Video::Geometry::Rectangle* CreateRectangle();
        
        /// Free the reference to the rectangle.
        /**
         * Deletes the instance if no more references exist.
         */
        void FreeRectangle();
        
        /// Create a cube for rendering if it doesn't already exist.
        /**
         * @return The cube instance
         */
        Geometry::Cube* CreateCube();
        
        /// Free the reference to the cube.
        /**
         * Deletes the instance if no more references exist.
         */
        void FreeCube();
        
        /// Create an model for rendering if it doesn't already exist.
        /**
        * @param filename Filename of model file.
        * @return The model instance
        */
        Geometry::Model* CreateModel(std::string filename);

        /// Free the reference to the model.
        /**
        * @param model %Model to dereference.
        */
        void FreeModel(Geometry::Model* model);
        
        /// Create a 2D texture if it doesn't already exist.
        /**
         * @param data Image file data.
         * @param dataLength Length of the image file data.
         * @param srgb Whether the image is in SRGB space and should be converted to linear space.
         * @return The %Texture2D instance
         */
        Video::Texture2D* CreateTexture2D(const char* data, int dataLength, bool srgb = false);
        
        /// Create a 2D texture if it doesn't already exist.
        /**
         * @param filename Filename of image file.
         * @param srgb Whether the image is in SRGB space and should be converted to linear space.
         * @return The %Texture2D instance
         */
        Video::Texture2D* CreateTexture2DFromFile(std::string filename, bool srgb = false);
        
        /// Free the reference to the 2D texture.
        /**
         * Deletes the instance if no more references exist.
         * @param texture %Texture to dereference.
         */
        void FreeTexture2D(Video::Texture2D* texture);
        
        /// Create a sound if it doesn't already exist.
        /**
         * Supported formats: Ogg Vorbis.
         * @param filename Path to the sound file.
         * @return The %SoundBuffer instance.
         */
        Audio::SoundBuffer* CreateSound(std::string filename);
        
        /// Free the reference to the sound.
        /**
         * Deletes the instance if no more references exist.
         * @param soundBuffer %SoundBuffer to dereference.
         */
        void FreeSound(Audio::SoundBuffer* soundBuffer);
        
    private:
        ResourceManager();
        ResourceManager(ResourceManager const&) = delete;
        void operator=(ResourceManager const&) = delete;
        
        // Shaders
        struct ShaderInstance {
            Video::Shader* shader;
            int count;
        };
        std::map<const char*, ShaderInstance> shaders;
        std::map<Video::Shader*, const char*> shadersInverse;
        
        // ShaderPrograms
        struct ShaderProgramInstance {
            Video::ShaderProgram* shaderProgram;
            int count;
        };
        struct ShaderProgramKey {
            const Video::Shader* computeShader = nullptr;
            const Video::Shader* vertexShader = nullptr;
            const Video::Shader* tessControlShader = nullptr;
            const Video::Shader* tessEvaluationShader = nullptr;
            const Video::Shader* geometryShader = nullptr;
            const Video::Shader* fragmentShader = nullptr;
            
            bool operator<(const ShaderProgramKey& other) const;
        };
        std::map<ShaderProgramKey, ShaderProgramInstance> shaderPrograms;
        std::map<Video::ShaderProgram*, ShaderProgramKey> shaderProgramsInverse;
        
        // Rectangle
        Video::Geometry::Rectangle* rectangle;
        int rectangleCount = 0;
        
        // Cube
        Geometry::Cube* cube;
        int cubeCount = 0;
        
        // Model
        struct ModelInstance {
            Geometry::Model* model;
            int count;
        };
        std::map<std::string, ModelInstance> models;
        std::map<Geometry::Model*, std::string> modelsInverse;
        
        // Texture2D
        struct Texture2DInstance {
            Video::Texture2D* texture;
            int count;
        };
        std::map<const char*, Texture2DInstance> textures;
        std::map<Video::Texture2D*, const char*> texturesInverse;
        
        // Texture2D from file
        std::map<std::string, Texture2DInstance> texturesFromFile;
        std::map<Video::Texture2D*, std::string> texturesFromFileInverse;
        
        // Sound
        struct SoundInstance {
            Audio::SoundBuffer* soundBuffer;
            int count;
        };
        std::map<std::string, SoundInstance> sounds;
        std::map<Audio::SoundBuffer*, std::string> soundsInverse;
};
