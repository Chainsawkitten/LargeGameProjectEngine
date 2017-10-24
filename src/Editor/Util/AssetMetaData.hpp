#pragma once

#include <fstream>
#include <ctime>

/// Static class for metadata generation.
/**
 * A metadata file contains information about a file.
 * The extension for a meta file is .meta.
 * Current metadata support exists for:
 * Meshes
 * 
 * @todo To be implemented:
 * Materials
 * Animations
 * Skeletons
 */
class AssetMetaData {
    public:
    /// Metadata for mesh asset.
    struct MeshImportData {
        char assetPath[128];
        char sourcePath[128];
        time_t saveTime;
        bool triangulate;
        bool importNormals;
        bool importTangents;
    };

    /// Generate metadata for a mesh.
    /**
         * @param filepath Filepath with extensions.
         * @param metaData Metadata to be exported.
         * @return False if file could not be opened.
         */
    static bool GenerateMetaData(const char* filepath, MeshImportData* metaData);

    /// Get metadata from a mesh.
    /**
         * @param filepath Filepath with extensions.
         * @return Metadata pointer with info about the file. You will have to delete it yourself.
         */
    static MeshImportData* GetMetaData(const char* filepath);

    protected:
    /// Constructor.
    AssetMetaData() {}

    /// Destructor.
    ~AssetMetaData() {}
};
