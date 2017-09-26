#pragma once

#include <fstream>
#include <cstdint>
#include <assert.h>
#include <vector>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Video/Geometry/VertexType/SkinVertex.hpp>

namespace Geometry {
    /// Handler for .asset format.
    /**
     * Start by using the Open() function.
     * The Open() function requries a filepath and
     * a mode READ/WRITE.
     * End by using the Close() function.
     */
    class AssetFileHandler {
        public:
            /// Mode to use class. READ/WRITE.
            enum Mode {
                READ = 0,
                WRITE = 1
            };

            /// Mesh data.
            struct MeshData {
                uint32_t parent;
                uint32_t numVertices;
                uint32_t numIndices;
                glm::vec3 aabbDim;
                glm::vec3 aabbOrigin;
                glm::vec3 aabbMinpos;
                glm::vec3 aabbMaxpos;
                bool isSkinned = false;
                Video::Geometry::VertexType::StaticVertex * staticVertices = nullptr;
                Video::Geometry::VertexType::SkinVertex * skinnedVertices = nullptr;
                uint32_t * indices = nullptr;

                ~MeshData() {
                    if (staticVertices != nullptr) {
                        delete[] staticVertices;
                    }
                    if (skinnedVertices != nullptr) {
                        delete[] skinnedVertices;
                    }
                    if (indices != nullptr) {
                        delete[] indices;
                    }
                }
            };

            /// The current version of the exporter.
            const uint16_t CURRENT_VERSION = 1;

            /// Importing is supported from version.
            const uint16_t SUPPORTED_FROM = 1;

            /// Default constructor.
            AssetFileHandler();

            /// Open a .asset file.
            /**
             * @param filepath Path of the file.
             * @param mode Use READ or WRITE.
             */
            AssetFileHandler(const char* filepath, Mode mode = READ);

            /// Destructor.
            ~AssetFileHandler();

            /// Open a .asset file.
            /**
             * @param filepath Path of the file.
             */
            bool Open(const char* filepath, Mode mode = READ);

            /// Close the opened file and clear data.
            void Close();

            /// Clear current data.
            void Clear();

            /// Load a mesh into memory.
            /**
            * @param Id of the mesh. Use GetNumMeshes() to get the number of meshes.
            */
            void LoadMeshData(int meshID);

            /// Get static vertex data of a mesh.
            /**
             * First load a mesh into memory by using LoadMeshData().
             * @return Static mesh data.
             */
            MeshData * GetStaticMeshData();

            /// Save the meshdata.
           /**
            * @param meshData Static mesh data.
            */
            void SaveStaticMesh(AssetFileHandler::MeshData * meshData);

        private:
            void ReadGlobalHeader();
            void WriteGlobalHeader();
            void ClearMesh();

            Mode mode;

            uint16_t uniqueID;
            uint16_t numStaticMeshes;

            MeshData * meshData = nullptr;

            std::ifstream rFile;
            std::ofstream wFile;
            int fileVersion;

            std::streampos globalHeaderStart;
            std::vector<std::streampos> staticMeshHeaderStart;
    };
}
