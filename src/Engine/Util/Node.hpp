#pragma once

struct Node {
    uint32_t index;
    char name[128];
    uint32_t numInputSlots = 0;
    uint32_t inputIndex[8];
    uint32_t numOutputSlots = 0;
    uint32_t outputIndex[8];
    glm::vec2 pos = glm::vec2(0.0f);
    glm::vec2 size = glm::vec2(250, 100);

    virtual void Save(std::ofstream* file) {
        file->write(reinterpret_cast<char*>(&index), sizeof(uint32_t));
        file->write(reinterpret_cast<char*>(name), 128);
        file->write(reinterpret_cast<char*>(&numInputSlots), sizeof(uint32_t));
    }

    virtual void Load(std::ifstream* file) {
        file->read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
        file->read(reinterpret_cast<char*>(name), 128);
        file->read(reinterpret_cast<char*>(&numInputSlots), sizeof(uint32_t));
    }
};