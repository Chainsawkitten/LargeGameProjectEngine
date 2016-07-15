#include "ResourceList.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Geometry/Cube.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Scene/Scene.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>
#include <Engine/Component/Mesh.hpp>

using namespace GUI;

ResourceList::ResourceList(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
}

ResourceList::~ResourceList() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
}

void ResourceList::Update() {
    double xpos = Input()->CursorX();
    double ypos = Input()->CursorY();
    
    bool mouseHover = xpos >= GetPosition().x && xpos < GetPosition().x + size.x && ypos >= GetPosition().y && ypos < GetPosition().y + size.y;
    
    if (mouseHover && Input()->MousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
        Entity* cube = Hymn().activeScene.CreateEntity();
        cube->AddComponent<Component::Transform>();
        Component::Mesh* cubeMesh = cube->AddComponent<Component::Mesh>();
        cubeMesh->geometry = Managers().resourceManager->CreateCube();
    }
}

void ResourceList::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    glm::vec2 position = GetPosition();
    rectangle->Render(position, size, color, screenSize);
    
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText("Entities", position, GetSize().x, screenSize);
    position.y += font->GetHeight();
    
    unsigned int id = 0;
    for (Entity* entity : Hymn().activeScene.GetEntities()) {
        font->RenderText(("Entity #" + std::to_string(id)).c_str(), position + glm::vec2(20.f, 0.f), GetSize().x, screenSize);
        position.y += font->GetHeight();
        ++id;
    }
}

glm::vec2 ResourceList::GetSize() const {
    return size;
}

void ResourceList::SetSize(const glm::vec2& size) {
    this->size = size;
}
