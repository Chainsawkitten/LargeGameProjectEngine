#include "StaticModel.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MathFunctions.hpp"
#include <Utility/Log.hpp>
#include "../Util/FileSystem.hpp"

using namespace Geometry;

StaticModel::StaticModel() {
    
}

StaticModel::StaticModel(const char* filename) {
    Load(filename);
}

StaticModel::~StaticModel() {
    
}

void StaticModel::Load(const char* filename) {
    if (assetFile.Open(filename, AssetFileHandler::READ)) {
        assetFile.LoadMeshData(0);
        AssetFileHandler::StaticMeshData * meshData = assetFile.GetStaticMeshData();
        GenerateVertexBuffer(vertexBuffer, meshData->vertices, meshData->numVertices);
        GenerateIndexBuffer(meshData->indices, meshData->numIndices, indexBuffer);
        GenerateVertexArray(vertexBuffer, indexBuffer, vertexArray);
        CreateAxisAlignedBoundingBox(meshData->aabbDim, meshData->aabbOrigin, meshData->aabbMinpos, meshData->aabbMaxpos);
        assetFile.Close();
    }
}

Video::Geometry::Geometry3D::Type StaticModel::GetType() const {
    return STATIC;
}

void StaticModel::GenerateVertexBuffer(GLuint& vertexBuffer, 
    Video::Geometry::VertexType::StaticVertex * vertices, unsigned int numVerticies) {
    vertexBuffer = Video::Geometry::VertexType::StaticVertex::GenerateVertexBuffer(
        vertices, numVerticies);
}

void StaticModel::GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) {
    vertexArray = Video::Geometry::VertexType::StaticVertex::GenerateVertexArray(vertexBuffer, indexBuffer);
}
