#pragma once

#include <map>
#include <GL/glew.h>
#include "../linking.hpp"

namespace Video {
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

namespace Animation {
    class AnimationClip;
    class AnimationController;
    class Skeleton;
}
class TextureAsset;
class ScriptFile;

/// Handles all resources.
class ResourceManager {
    friend class Hub;
    
    public:
        /// Constructor
        ResourceManager() {}

        /// Create an animation clip.
        /**
         * @param name Name of animation clip.
         * @return The animation clip instance.
         */
        ENGINE_API Animation::AnimationClip* CreateAnimationClip(const std::string& name);

        /// Free the reference to the animation clip.
        /**
         * @param animationClip %Animation clip to dereference.
         */
        ENGINE_API void FreeAnimationClip(Animation::AnimationClip * animationClip);

        /// Create an animation clip.
        /**
         * @param name Name of animation controller.
         * @return The animation clip instance.
         */
        ENGINE_API Animation::AnimationController* CreateAnimationController(const std::string& name);

        /// Free the reference to the animation clip.
        /**
         * @param animationClip %Animation clip to dereference.
         */
        ENGINE_API void FreeAnimationController(Animation::AnimationController * animationController);

        /// Create a skeleton.
        /**
         * @param name Name of skeleton.
         * @return The skeleton instance.
         */
        ENGINE_API Animation::Skeleton* CreateSkeleton(const std::string& name);

        /// Free the reference to the skeleton.
        /**
         * @param skeleton %Skeleton to dereference.
         */
        ENGINE_API void FreeSkeleton(Animation::Skeleton * skeleton);

        /**
         * @param name Name of model.
         * @return The model instance
         */
        ENGINE_API Geometry::Model* CreateModel(const std::string& name);

        /// Free the reference to the model.
        /**
        * @param model %Model to dereference.
        */
        ENGINE_API void FreeModel(Geometry::Model* model);
        
        /// Create a 2D texture if it doesn't already exist.
        /**
         * @param data Image file data.
         * @param dataLength Length of the image file data.
         * @param srgb Whether the image is in SRGB space and should be converted to linear space.
         * @return The %Texture2D instance
         */
        ENGINE_API Video::Texture2D* CreateTexture2D(const char* data, int dataLength, bool srgb = false);
        
        /// Free the reference to the 2D texture.
        /**
         * Deletes the instance if no more references exist.
         * @param texture %Texture to dereference.
         */
        ENGINE_API void FreeTexture2D(Video::Texture2D* texture);
        
        /// Create a texture asset if it doesn't already exist.
        /**
         * @param name The name of the texture asset.
         * @return The %TextureAsset instance
         */
        ENGINE_API TextureAsset* CreateTextureAsset(const std::string& name);
        
        /// Free the reference to the texture asset.
        /**
         * Deletes the instance if no more references exist.
         * @param textureAsset %TextureAsset to dereference.
         */
        ENGINE_API void FreeTextureAsset(TextureAsset* textureAsset);
        
        /// Get the number of instances of a texture asset.
        /**
         * @param textureAsset The texture asset to check.
         * @return How many instances of the texture asset currently exist.
         */
        ENGINE_API int GetTextureAssetInstanceCount(TextureAsset* textureAsset);
        
        /// Create a sound if it doesn't already exist.
        /**
         * @param name Name of the sound.
         * @return The %SoundBuffer instance.
         */
        ENGINE_API Audio::SoundBuffer* CreateSound(const std::string& name);
        
        /// Free the reference to the sound.
        /**
         * Deletes the instance if no more references exist.
         * @param soundBuffer %SoundBuffer to dereference.
         */
        ENGINE_API void FreeSound(Audio::SoundBuffer* soundBuffer);
        
        /// Create a script file if it doesn't already exist.
        /**
         * @param name Name of the script file.
         * @return The %ScriptFile instance.
         */
        ENGINE_API ScriptFile* CreateScriptFile(const std::string& name);
        
        /// Free the reference to the script file.
        /**
         * Deletes the instance if no more references exist.
         * @param scriptFile %ScriptFile to dereference.
         */
        ENGINE_API void FreeScriptFile(ScriptFile* scriptFile);
        
    private:
        ResourceManager(ResourceManager const&) = delete;
        void operator=(ResourceManager const&) = delete;
        
        // Rectangle.
        Video::Geometry::Rectangle* rectangle;
        int rectangleCount = 0;
        
        // Cube.
        Geometry::Cube* cube;
        int cubeCount = 0;
        
        // Model.
        struct ModelInstance {
            Geometry::Model* model;
            int count;
        };
        std::map<std::string, ModelInstance> models;
        std::map<Geometry::Model*, std::string> modelsInverse;

        // Animation clip.
        struct AnimationClipInstance {
            Animation::AnimationClip* animationClip;
            int count;
        };
        std::map<std::string, AnimationClipInstance> animationClips;
        std::map<Animation::AnimationClip*, std::string> animationClipsInverse;

        // Animation controller.
        struct AnimationControllerInstance {
            Animation::AnimationController* animationController;
            int count;
        };
        std::map<std::string, AnimationControllerInstance> animationControllers;
        std::map<Animation::AnimationController*, std::string> animationControllersInverse;

        // Skeleton.
        struct SkeletonInstance {
            Animation::Skeleton* skeleton;
            int count;
        };
        std::map<std::string, SkeletonInstance> skeletons;
        std::map<Animation::Skeleton*, std::string> skeletonsInverse;

        // Texture2D.
        struct Texture2DInstance {
            Video::Texture2D* texture;
            int count;
        };
        std::map<const char*, Texture2DInstance> textures;
        std::map<Video::Texture2D*, const char*> texturesInverse;
        
        // Texture asset.
        struct TextureAssetInstance {
            TextureAsset* textureAsset;
            int count;
        };
        std::map<std::string, TextureAssetInstance> textureAssets;
        std::map<TextureAsset*, std::string> textureAssetsInverse;
        
        // Sound.
        struct SoundInstance {
            Audio::SoundBuffer* soundBuffer;
            int count;
        };
        std::map<std::string, SoundInstance> sounds;
        std::map<Audio::SoundBuffer*, std::string> soundsInverse;
        
        // ScriptFile.
        struct ScriptFileInstance {
            ScriptFile* scriptFile;
            int count;
        };
        std::map<std::string, ScriptFileInstance> scriptFiles;
        std::map<ScriptFile*, std::string> scriptFilesInverse;
};
