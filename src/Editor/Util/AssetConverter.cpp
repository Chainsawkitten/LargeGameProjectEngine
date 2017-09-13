#include "AssetConverter.hpp"
#include <Utility/Log.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>

AssetConverter::AssetConverter() {

}

AssetConverter::~AssetConverter() {

}

bool AssetConverter::Convert(const char * filepath, const char * destination,
    bool triangulate, bool importNormals, bool importTangents) {
    Geometry::AssetFileHandler file;

    // Return if file is not open.
    if (!file.Open(destination, Geometry::AssetFileHandler::WRITE))
        return false;

    unsigned int flags = triangulate ? aiProcess_Triangulate : 0;
    flags = importNormals ? flags : flags | aiProcess_CalcTangentSpace;
    flags = importTangents ? flags : flags | aiProcess_GenSmoothNormals;

    const aiScene* aScene = aImporter.ReadFile(filepath, flags);

    if (aScene == nullptr) {
        Log() << "Error importing mesh: " << filepath << "\n";
        Log() << aImporter.GetErrorString() << "\n";
    }

    ConvertMeshes(aScene, &file);

    aImporter.FreeScene();

    file.Close();
    return true;
}

void AssetConverter::ConvertMeshes(const aiScene * aScene, Geometry::AssetFileHandler * file) {
    for (unsigned int i = 0; i < aScene->mNumMeshes; ++i) {
        ConvertMesh(aScene->mMeshes[i], file);
    }
}

void AssetConverter::ConvertMesh(aiMesh * aMesh, Geometry::AssetFileHandler * file) {
    // Convert vertices.
    unsigned int numVertices = aMesh->mNumVertices;
    Video::Geometry::VertexType::StaticVertex * vertices = new Video::Geometry::VertexType::StaticVertex[numVertices];
    for (int i = 0; i < numVertices; ++i) {
        Geometry::CpyVec(vertices[i].position, aMesh->mVertices[i]);
        Geometry::CpyVec(vertices[i].textureCoordinate, aMesh->mTextureCoords[0][i]);
        Geometry::CpyVec(vertices[i].normal, aMesh->mNormals[i]);
        Geometry::CpyVec(vertices[i].tangent, aMesh->mTangents[i]);
    }

    // Convert indicies. 3 indicies for a face/triangle.
    unsigned int numIndicies = aMesh->mNumFaces * 3;
    uint32_t * indices = new uint32_t[numIndicies];
    unsigned int indexCounter = 0;
    for (int i = 0; i < aMesh->mNumFaces; ++i) {
        const aiFace& aFace = aMesh->mFaces[i];
        if (aFace.mNumIndices != 3) {
            Log() << "Error importing mesh. Face that doesn't have 3 indices. Indices: " << aFace.mNumIndices << "\n";
        }

        indices[indexCounter++] = aFace.mIndices[0];
        indices[indexCounter++] = aFace.mIndices[1];
        indices[indexCounter++] = aFace.mIndices[2];
    }

    // Fix aabb.
    glm::vec3 minValues, maxValues, origin, dim;
    minValues = maxValues = origin = glm::vec3(0.f, 0.f, 0.f);

    // Find minimum/maximum bounding points.
    for (std::size_t i = 0; i < numVertices; ++i) {
        const glm::vec3& pos = vertices[i].position;
        if (pos.x > maxValues.x)
            maxValues.x = pos.x;
        else if (pos.x < minValues.x)
            minValues.x = pos.x;

        if (pos.y > maxValues.y)
            maxValues.y = pos.y;
        else if (pos.y < minValues.y)
            minValues.y = pos.y;

        if (pos.z > maxValues.z)
            maxValues.z = pos.z;
        else if (pos.z < minValues.z)
            minValues.z = pos.z;
    }

    // Set origin.
    origin.x = (minValues.x + maxValues.x) / 2.f;
    origin.y = (minValues.y + maxValues.y) / 2.f;
    origin.z = (minValues.z + maxValues.z) / 2.f;

    // Dimensions.
    dim.x = maxValues.x - minValues.x;
    dim.y = maxValues.y - minValues.y;
    dim.z = maxValues.z - minValues.z;

    Geometry::AssetFileHandler::StaticMeshData * meshData = new Geometry::AssetFileHandler::StaticMeshData;
    meshData->parent = 0;
    meshData->numVertices = numVertices;
    meshData->numIndices = indexCounter;
    meshData->vertices = vertices;
    meshData->indices = indices;
    meshData->aabbDim = dim;
    meshData->aabbOrigin = origin;
    meshData->aabbMinpos = minValues;
    meshData->aabbMaxpos = maxValues;

    file->SaveStaticMesh(meshData);

    delete meshData;
    meshData = nullptr;
}
