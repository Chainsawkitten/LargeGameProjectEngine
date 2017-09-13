#include "AssetFileHandler.hpp"
#include <Utility/Log.hpp>

using namespace Geometry;

AssetFileHandler::AssetFileHandler() {

}

AssetFileHandler::AssetFileHandler(const char* filepath, Mode mode) {
    Open(filepath, mode);
}

AssetFileHandler::~AssetFileHandler() {
    Close();
}

bool AssetFileHandler::Open(const char* filepath, Mode mode) {
    // Close the file is already open.
    Close();

    this->mode = mode;

    if (mode == READ) {
        // Open the .asset file
        rFile.open(filepath, std::ios::binary);

        // Return false if file is not open.
        if (!rFile.is_open())
            return false;

        // Read version.
        rFile.read(reinterpret_cast<char*>(&fileVersion), sizeof(uint16_t));

        ReadGlobalHeader();
    }
    else {
        // Create the .asset file.
        wFile.open(filepath, std::ios::binary);
        if (!wFile.is_open()) {
            Log() << "Could not create file: " << filepath << "\n";
            return false;
        }

        // Write version.
        uint16_t verison = CURRENT_VERSION;
        wFile.write(reinterpret_cast<char*>(&verison), sizeof(uint16_t));

        uniqueID = 1;
        numStaticMeshes = 1;
        WriteGlobalHeader();
    }

    return true;
}

void AssetFileHandler::Close() {
    if (rFile.is_open())
        rFile.close();

    if (wFile.is_open())
        wFile.close();

    Clear();
}

void AssetFileHandler::Clear() {
    ClearMesh();
}

void AssetFileHandler::LoadMeshData(int meshID) {
    ClearMesh();

    staticMesh = new StaticMeshData();

    rFile.read(reinterpret_cast<char*>(&staticMesh->parent), sizeof(uint32_t));
    rFile.read(reinterpret_cast<char*>(&staticMesh->numVertices), sizeof(uint32_t));
    rFile.read(reinterpret_cast<char*>(&staticMesh->numIndices), sizeof(uint32_t));
    rFile.read(reinterpret_cast<char*>(&staticMesh->aabbDim), sizeof(glm::vec3));
    rFile.read(reinterpret_cast<char*>(&staticMesh->aabbOrigin), sizeof(glm::vec3));
    rFile.read(reinterpret_cast<char*>(&staticMesh->aabbMinpos), sizeof(glm::vec3));
    rFile.read(reinterpret_cast<char*>(&staticMesh->aabbMaxpos), sizeof(glm::vec3));

    staticMesh->vertices = new Video::Geometry::VertexType::StaticVertex[staticMesh->numVertices];
    rFile.read(reinterpret_cast<char*>(staticMesh->vertices), sizeof(Video::Geometry::VertexType::StaticVertex) * staticMesh->numVertices);

    staticMesh->indices = new uint32_t[staticMesh->numIndices];
    rFile.read(reinterpret_cast<char*>(staticMesh->indices), sizeof(uint32_t) * staticMesh->numIndices);
}

AssetFileHandler::StaticMeshData * AssetFileHandler::GetStaticMeshData() {
    return staticMesh;
}

void AssetFileHandler::SaveStaticMesh(AssetFileHandler::StaticMeshData * meshData) {
    // Write header.
    wFile.write(reinterpret_cast<char*>(&meshData->parent), sizeof(uint32_t));
    wFile.write(reinterpret_cast<char*>(&meshData->numVertices), sizeof(uint32_t));
    wFile.write(reinterpret_cast<char*>(&meshData->numIndices), sizeof(uint32_t));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbDim), sizeof(glm::vec3));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbOrigin), sizeof(glm::vec3));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbMinpos), sizeof(glm::vec3));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbMaxpos), sizeof(glm::vec3));

    // Write mesh data.
    wFile.write(reinterpret_cast<char*>(meshData->vertices), sizeof(Video::Geometry::VertexType::StaticVertex) * meshData->numVertices);
    wFile.write(reinterpret_cast<char*>(meshData->indices), sizeof(uint32_t) * meshData->numIndices);
}

void AssetFileHandler::ReadGlobalHeader() {
    rFile.read(reinterpret_cast<char*>(&uniqueID), sizeof(uint16_t));
    rFile.read(reinterpret_cast<char*>(&numStaticMeshes), sizeof(uint16_t));
}

void AssetFileHandler::WriteGlobalHeader() {
    wFile.write(reinterpret_cast<char*>(&uniqueID), sizeof(uint16_t));
    wFile.write(reinterpret_cast<char*>(&numStaticMeshes), sizeof(uint16_t));
}

void AssetFileHandler::ClearMesh() {
    if (staticMesh != nullptr) {
        delete staticMesh;
    }

    staticMesh = nullptr;
}
