#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include "Entity/World.hpp"

class TextureAsset;
class ScriptFile;

/// A hymn to beauty.
class ActiveHymn {
    friend ActiveHymn& Hymn();
    
    public:
        /// Clear the hymn of all properties.
        void Clear();
        
        /// Get the path where the hymn is saved.
        /**
         * @return The hymn's path.
         */
        const std::string& GetPath() const;
        
        /// Set the path where the hymn is saved.
        /**
         * @param path New path.
         */
        void SetPath(const std::string& path);

        /// Gets the path to the hymn file.
        /**
         * @return The full path.
         */
        std::string GetSavePath() const;

        /// Save the hymn.
        void Save() const;
        
        /// Load a hymn.
        /**
         * @param path Path to the saved hymn.
         */
        void Load(const std::string& path);

        /// Convert the hymn to Json.
        /**
         * @return The hymn as a Json.
         */
        Json::Value ToJson() const;

        /// Convert a Json to a Hymn.
        /**
         * @param root The Json file to load.
         */
        void FromJson(Json::Value root);

        /// Update the world.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(float deltaTime);
        
        /// Render the world.
        /**
         * @param camera Camera through which to render (or first camera in the world if nullptr).
         * @param soundSources Whether to show sound sources.
         * @param particleEmitters Whether to show particle emitters.
         * @param lightSources Whether to show light sources.
         * @param cameras Whether to show cameras.
         * @param physics Whether to show physics volumes.
         */
        void Render(Entity* camera = nullptr, bool soundSources = false, bool particleEmitters = false, bool lightSources = false, bool cameras = false, bool physics = false, bool gridSettings = false);

        /// Create static grid.
        /**
        * @param scale Scales the grid, scale can be a maximum of 100.
        */
        void CreateGrid(int scale);

        /// The game world.
        World world;
        
        /// The id of the next entity to create.
        unsigned int entityNumber = 1U;
        
        /// Scripts.
        std::vector<ScriptFile*> scripts;
    
        /// The id of the next script to create.
        unsigned int scriptNumber = 0U;

        /// Default albedo texture.
        TextureAsset* defaultAlbedo;
        
        /// Default normal texture.
        TextureAsset* defaultNormal;
        
        /// Default metallic texture.
        TextureAsset* defaultMetallic;
        
        /// Default roughness texture.
        TextureAsset* defaultRoughness;
        
        /// Grid settings.
        struct GridSettings
        {
            int gridScale = 0;
            bool gridSettingsOpen = true;

        };

        GridSettings gridSettings;

        /// Filter settings.
        struct FilterSettings {
            /// Whether to enable color.
            bool color = false;
            
            /// The color to blend with.
            glm::vec3 colorColor = glm::vec3(1.0f, 1.0f, 1.0f);
            
            /// Whether to enable fog.
            bool fog = false;
            
            /// Fog density.
            float fogDensity = 0.01f;
            
            /// Fog color.
            glm::vec3 fogColor = glm::vec3(1.0f, 1.0f, 1.0f);
            
            /// Whether to enable FXAA.
            bool fxaa = true;
            
            /// Whether to enable glow.
            bool glow = true;
            
            /// How many times to blur the glow buffer.
            int glowBlurAmount = 1;
        };
        
        /// Filter settings.
        FilterSettings filterSettings;
        
    private:
        static ActiveHymn& GetInstance();
        
        ActiveHymn();
        ActiveHymn(ActiveHymn const&) = delete;
        void operator=(ActiveHymn const&) = delete;
        
        std::string path = "";
};

/// Get the active hymn.
/**
 * @return The %ActiveHymn instance.
 */
ActiveHymn& Hymn();
