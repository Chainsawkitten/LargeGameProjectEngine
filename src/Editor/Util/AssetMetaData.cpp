#include "AssetMetaData.hpp"

bool AssetMetaData::GenerateMetaData(const char* filepath, MeshImportData* meshImportData) {
    // Open file for writing.
    std::ofstream file(filepath);

    // Return if file is not open.
    if (!file.is_open())
        return false;

    // Write to file.
    file.write(reinterpret_cast<char*>(meshImportData), sizeof(MeshImportData));

    // Close the file.
    file.close();
}

AssetMetaData::MeshImportData* AssetMetaData::GetMetaData(const char* filepath) {
    // Open file for reading.
    std::ifstream file(filepath);

    // Return if file is not open.
    if (!file.is_open())
        return false;

    MeshImportData* meshImportData = new MeshImportData;

    // Read the file.
    file.read(reinterpret_cast<char*>(meshImportData), sizeof(MeshImportData));

    // Close the file.
    file.close();

    return meshImportData;
}
