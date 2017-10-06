#pragma once

#include <vector>
#include <string>

class TextureAsset;
class ScriptFile;
namespace Geometry {
    class Model;
}
namespace Audio {
    class SoundBuffer;
}

/// A list of all the resources in a hymn.
class ResourceList {
    friend ResourceList& Resources();
    
    public:
        /// Save all resources to file.
        void Save() const;
        
        /// Load all resources from file.
        void Load();
        
        /// Clear resources.
        void Clear();
        
        /// Scenes.
        std::vector<std::string> scenes;
        
        /// The index to the activeScene.
        int activeScene;
        
        /// Models.
        std::vector<Geometry::Model*> models;
        
        /// The id of the next model to create.
        unsigned int modelNumber = 0U;
        
        /// Textures.
        std::vector<TextureAsset*> textures;
        
        /// The id of the next texture to create.
        unsigned int textureNumber = 0U;
        
        /// Sounds.
        std::vector<Audio::SoundBuffer*> sounds;
    
        /// The id of the next sound to create.
        unsigned int soundNumber = 0U;
        
    private:
        static ResourceList& GetInstance();
        
        ResourceList();
        ResourceList(ResourceList const&) = delete;
        void operator=(ResourceList const&) = delete;
};

/// Get the resource list.
/**
 * @return The %ResourceList instance.
 */
ResourceList& Resources();
