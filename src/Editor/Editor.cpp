#include "Editor.hpp"

#include "Util/EditorSettings.hpp"
#undef CreateDirectory

#include <Engine/Util/Input.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/DebugDrawingManager.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/MainWindow.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/Lens.hpp>
#include <Engine/Component/Listener.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Geometry/Model.hpp>
#include "ImGui/Theme.hpp"
#include "Resources.hpp"
#include <ImGuizmo.h>
#include <imgui.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/transform.hpp>
#include <fstream>
#include <Utility/Log.hpp>


ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
Editor::Editor() {
    // Create Hymns directory.
    FileSystem::CreateDirectory((FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns").c_str());

    // Load theme.
    std::string theme = EditorSettings::GetInstance().GetString("Theme");
    if (FileSystem::FileExists((FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes" + FileSystem::DELIMITER + theme + ".json").c_str()))
        ImGui::LoadTheme(theme.c_str());
    else
        ImGui::LoadDefaultTheme();

    selectedEntity = nullptr;

    // Assign controls.
    Input()->AssignButton(InputHandler::PROFILE, InputHandler::KEYBOARD, GLFW_KEY_F2);
    Input()->AssignButton(InputHandler::PLAYTEST, InputHandler::KEYBOARD, GLFW_KEY_F5);
    Input()->AssignButton(InputHandler::CONTROL, InputHandler::KEYBOARD, GLFW_KEY_LEFT_CONTROL);
    Input()->AssignButton(InputHandler::NEW, InputHandler::KEYBOARD, GLFW_KEY_N);
    Input()->AssignButton(InputHandler::OPEN, InputHandler::KEYBOARD, GLFW_KEY_O);
    Input()->AssignButton(InputHandler::SAVE, InputHandler::KEYBOARD, GLFW_KEY_S);
    Input()->AssignButton(InputHandler::CAMERA, InputHandler::MOUSE, GLFW_MOUSE_BUTTON_MIDDLE);
    Input()->AssignButton(InputHandler::FORWARD, InputHandler::KEYBOARD, GLFW_KEY_W);
    Input()->AssignButton(InputHandler::BACKWARD, InputHandler::KEYBOARD, GLFW_KEY_S);
    Input()->AssignButton(InputHandler::LEFT, InputHandler::KEYBOARD, GLFW_KEY_A);
    Input()->AssignButton(InputHandler::RIGHT, InputHandler::KEYBOARD, GLFW_KEY_D);
    Input()->AssignButton(InputHandler::ZOOM, InputHandler::KEYBOARD, GLFW_KEY_Z);
    Input()->AssignButton(InputHandler::SELECT, InputHandler::MOUSE, GLFW_MOUSE_BUTTON_LEFT);
    Input()->AssignButton(InputHandler::FOCUS, InputHandler::KEYBOARD, GLFW_KEY_F);
    Input()->AssignButton(InputHandler::W, InputHandler::KEYBOARD, GLFW_KEY_W);
    Input()->AssignButton(InputHandler::E, InputHandler::KEYBOARD, GLFW_KEY_E);
    Input()->AssignButton(InputHandler::R, InputHandler::KEYBOARD, GLFW_KEY_R);

    // Create editor camera.
    cameraEntity = cameraWorld.CreateEntity("Editor Camera");
    cameraEntity->enabled = false;
    cameraEntity->AddComponent<Component::Lens>();
    cameraEntity->position.z = 10.0f;
    cameraEntity->GetComponent<Component::Lens>()->zFar = 1000.f;

    // Create cursors.
    cursors[0] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    cursors[1] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    cursors[2] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    cursors[3] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    cursors[4] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);

    savePromptAnswered = false;
    savePromtWindow.SetTitle("Save before you quit?");
    close = false;
    
    // Load settings.
    showGridSettings = EditorSettings::GetInstance().GetBool("Grid Settings");
    gridSettings.gridSize = EditorSettings::GetInstance().GetLong("Grid Size");
    gridSettings.lineWidth = EditorSettings::GetInstance().GetLong("Grid Line Width");
    gridSettings.gridSnap = EditorSettings::GetInstance().GetBool("Grid Snap");
    gridSettings.snapOption = EditorSettings::GetInstance().GetLong("Grid Snap Size");

    // Ray mouse.
    mousePicker = MousePicking(cameraEntity, cameraEntity->GetComponent < Component::Lens>()->GetProjection(glm::vec2(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y)));
}

Editor::~Editor() {
    // Destroy cursors.
    for (int i = 0; i < 5; ++i) {
        glfwDestroyCursor(cursors[i]);
    }
}

void Editor::Show(float deltaTime) {
    if (close) {
        if (!HasMadeChanges()) {
            savePromptAnswered = true;
        } else {

            // Ask the user whether they wish to save.
            if (Hymn().GetPath() != "") {
                savePromtWindow.SetVisible(true);
                savePromtWindow.Show();

                switch (savePromtWindow.GetDecision()) {
                case 0:
                    Save();
                    savePromptAnswered = true;
                    break;

                case 1:
                    savePromptAnswered = true;
                    break;

                case 2:
                    savePromptAnswered = false;
                    close = false;
                    savePromtWindow.ResetDecision();
                    break;

                default:
                    break;
                }
            } else {
                savePromptAnswered = true;
            }
        }
    } else {
        bool play = false;
        
        // Main menu bar.
        ShowMainMenuBar(play);

        // Show hymn selection window.
        if (selectHymnWindow.IsVisible()) {
            ImGui::SetNextWindowPosCenter();
            selectHymnWindow.Show();
        }

        if (inputWindow.IsVisible())
            inputWindow.Show();

        // Show filters window.
        if (filtersWindow.IsVisible())
            filtersWindow.Show();

        // Show resource list.
        if (resourceView.IsVisible())
            resourceView.Show();

        // Show settings window.
        if (settingsWindow.IsVisible()) {
            settingsWindow.Show();
        }
        
        // Show grid settings window.
        ShowGridSettings();
        CreateGrid(gridSettings.gridSize);

        // Control the editor camera.
        ControlEditorCamera(deltaTime);

        // Select entity by clicking on it with the mouse.
        Picking();
        
        // Move camera position and rotation to fixate on selected object.
        Focus();
        
        // Scroll zoom.
        if (Input()->GetScrollDown()) {
            if (!ImGui::IsMouseHoveringAnyWindow()) {
                glm::mat4 orientation = cameraEntity->GetCameraOrientation();
                glm::vec3 backward(orientation[0][2], orientation[1][2], orientation[2][2]);
                float speed = 2.0f * deltaTime * glm::length(cameraEntity->position);
                cameraEntity->position += speed * backward;
            }
        }
        if (Input()->GetScrollUp()) {
            if (!ImGui::IsMouseHoveringAnyWindow()) {
                glm::mat4 orientation = cameraEntity->GetCameraOrientation();
                glm::vec3 backward(orientation[0][2], orientation[1][2], orientation[2][2]);
                float speed = 2.0f * deltaTime * glm::length(cameraEntity->position);
                cameraEntity->position += speed * -backward;
            }
        }

        if (Input()->Triggered(InputHandler::PLAYTEST) && Hymn().GetPath() != "")
            play = true;

        if (Input()->Triggered(InputHandler::NEW) && Input()->Pressed(InputHandler::CONTROL))
            NewHymn();

        if (Input()->Triggered(InputHandler::OPEN) && Input()->Pressed(InputHandler::CONTROL))
            OpenHymn();

        if (Hymn().GetPath() != "" && Input()->Triggered(InputHandler::SAVE) && Input()->Pressed(InputHandler::CONTROL))
            Save();

        if (play)
            Play();

    }

    // Set cursor.
    if (ImGui::GetMouseCursor() < 5) {
        glfwSetCursor(MainWindow::GetInstance()->GetGLFWWindow(), cursors[ImGui::GetMouseCursor()]);
    }

    // Widget Controller for translation, rotation  and scale.
    ImGuizmo::BeginFrame();
    ImGuizmo::Enable(true);
    //Widget operation is in local mode
    ImGuizmo::MODE currentGizmoMode(ImGuizmo::LOCAL);
    ImGuiIO& io = ImGui::GetIO();

    // Check that there is an active Entity.

    // Get current active Entity.
    glm::mat4 currentEntityMatrix = glm::mat4();

    Entity* currentEntity = resourceView.GetScene().entityEditor.GetEntity();

    if (currentEntity != nullptr) {
        currentEntityMatrix = currentEntity->GetLocalMatrix();

        // Change operation based on key input.
        if (Input()->Triggered(InputHandler::W))
            currentOperation = ImGuizmo::TRANSLATE;
        else if (Input()->Triggered(InputHandler::E))
            currentOperation = ImGuizmo::ROTATE;
        else if (Input()->Triggered(InputHandler::R))
            currentOperation = ImGuizmo::SCALE;

        // Projection matrix.
        glm::mat4 projectionMatrix = cameraEntity->GetComponent<Component::Lens>()->GetProjection(glm::vec2(io.DisplaySize.x, io.DisplaySize.y));

        // View matrix.
        glm::mat4 viewMatrix = cameraEntity->GetCameraOrientation() * glm::translate(glm::mat4(), -cameraEntity->GetWorldPosition());

        // Identity matrix.
        float translationValue[3] = { currentEntity->position.x, currentEntity->position.y, currentEntity->position.z };
        float scaleValue[3] = { currentEntity->scale.x, currentEntity->scale.y, currentEntity->scale.z };
        float rotationValue[3] = { currentEntity->rotation.x, currentEntity->rotation.y, currentEntity->rotation.z };

        // Draw the actual widget.
        ImGuizmo::SetRect(currentEntityMatrix[0][0], 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::RecomposeMatrixFromComponents(translationValue, rotationValue, scaleValue, &currentEntityMatrix[0][0]);
        ImGuizmo::Manipulate(&viewMatrix[0][0], &projectionMatrix[0][0], currentOperation, currentGizmoMode, &currentEntityMatrix[0][0]);
        ImGuizmo::DecomposeMatrixToComponents(&currentEntityMatrix[0][0], translationValue, rotationValue, scaleValue);
        glm::rotate(-90.0f, glm::vec3(rotationValue[0], rotationValue[1], rotationValue[2]));

        if (ImGuizmo::IsUsing()) {
            switch (currentOperation) {
            case ImGuizmo::TRANSLATE:
                currentEntity->position.x = translationValue[0];
                currentEntity->position.y = translationValue[1];
                currentEntity->position.z = translationValue[2];
            case ImGuizmo::ROTATE:
                currentEntity->rotation.x = rotationValue[0];
                currentEntity->rotation.y = rotationValue[1];
                currentEntity->rotation.z = rotationValue[2];
            case ImGuizmo::SCALE:
                currentEntity->scale.x = scaleValue[0];
                currentEntity->scale.y = scaleValue[1];
                currentEntity->scale.z = scaleValue[2];
            }
        }
    }
}

void Editor::Save() const {
    resourceView.SaveScene();
    Hymn().Save();
    Resources().Save();
}

bool Editor::HasMadeChanges() const {

    {
        std::string* sceneFilename = new std::string();
        Json::Value sceneJson = resourceView.GetSceneJson(sceneFilename);

        // Load Json document from file.
        Json::Value reference;
        std::ifstream file(*sceneFilename);

        if (!file.good())
            return true;

        file >> reference;
        file.close();

        std::string sceneJsonString = sceneJson.toStyledString();
        std::string referenceString = reference.toStyledString();

        int response = referenceString.compare(sceneJsonString);
        if (response != 0)
            return true;
    }
    {
        std::string hymnFilename = Hymn().GetSavePath();
        Json::Value hymnJson = Hymn().ToJson();

        // Load Json document from file.
        Json::Value reference;
        std::ifstream file(hymnFilename);

        if (!file.good())
            return true;

        file >> reference;
        file.close();

        std::string hymnJsonString = hymnJson.toStyledString();
        std::string referenceString = reference.toStyledString();

        int response = referenceString.compare(hymnJsonString);
        if (response != 0)
            return true;
    }
    {
        std::string resourcesFilename = Resources().GetSavePath();
        Json::Value resourcesJson = Resources().ToJson();

        // Load Json document from file.
        Json::Value reference;
        std::ifstream file(resourcesFilename);

        if (!file.good())
            return true;

        file >> reference;
        file.close();

        std::string resourcesJsonString = resourcesJson.toStyledString();
        std::string referenceString = reference.toStyledString();

        int response = referenceString.compare(resourcesJsonString);
        if (response != 0)
            return true;
    }

    return false;

}

bool Editor::ReadyToClose() const {
    return savePromptAnswered;
}

bool Editor::isClosing() const {
    return close;
}

void Editor::Close() {
    close = true;
}

bool Editor::IsVisible() const {
    return visible;
}

void Editor::SetVisible(bool visible) {
    this->visible = visible;
}

Entity* Editor::GetCamera() const {
    return cameraEntity;
}

void Editor::ShowMainMenuBar(bool& play) {
    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);
    
    // Main menu bar.
    if (ImGui::BeginMainMenuBar()) {
        // File menu.
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Hymn", "CTRL+N"))
                NewHymn();

            if (ImGui::MenuItem("Open Hymn", "CTRL+O"))
                OpenHymn();

            if (Hymn().GetPath() != "") {
                if (ImGui::MenuItem("Save Hymn", "CTRL+S"))
                    Save();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Settings"))
                settingsWindow.SetVisible(true);

            ImGui::EndMenu();
        }

        // View menu.
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Grid Settings", "", &showGridSettings);
            EditorSettings::GetInstance().SetBool("Grid Settings", showGridSettings);

            static bool soundSources = EditorSettings::GetInstance().GetBool("Sound Source Icons");
            ImGui::MenuItem("Sound Sources", "", &soundSources);
            EditorSettings::GetInstance().SetBool("Sound Source Icons", soundSources);

            static bool particleEmitters = EditorSettings::GetInstance().GetBool("Particle Emitter Icons");
            ImGui::MenuItem("Particle Emitters", "", &particleEmitters);
            EditorSettings::GetInstance().SetBool("Particle Emitter Icons", particleEmitters);

            static bool lightSources = EditorSettings::GetInstance().GetBool("Light Source Icons");
            ImGui::MenuItem("Light Sources", "", &lightSources);
            EditorSettings::GetInstance().SetBool("Light Source Icons", lightSources);

            static bool cameras = EditorSettings::GetInstance().GetBool("Camera Icons");
            ImGui::MenuItem("Cameras", "", &cameras);
            EditorSettings::GetInstance().SetBool("Camera Icons", cameras);

            static bool physics = EditorSettings::GetInstance().GetBool("Physics Volumes");
            ImGui::MenuItem("Physics", "", &physics);
            EditorSettings::GetInstance().SetBool("Physics Volumes", physics);

            ImGui::EndMenu();
        }

        if (Hymn().GetPath() != "") {
            // Play
            if (ImGui::BeginMenu("Play")) {
                if (ImGui::MenuItem("Play", "F5"))
                    play = true;

                ImGui::EndMenu();
            }

            // Hymn
            if (ImGui::BeginMenu("Hymn")) {
                if (ImGui::MenuItem("Input"))
                    inputWindow.SetVisible(true);

                if (ImGui::MenuItem("Filters"))
                    filtersWindow.SetVisible(true);

                ImGui::EndMenu();
            }

            if (Input()->Triggered(InputHandler::ZOOM)) {
                if (resourceView.GetScene().entityEditor.GetEntity() != nullptr) {
                    const glm::vec3 tempPos = resourceView.GetScene().entityEditor.GetEntity()->GetWorldPosition();
                    cameraEntity->position = tempPos + glm::vec3(0, 7, 7);
                    cameraEntity->rotation = glm::vec3(0, 45, 1);
                }
            }

            // Editor Camera Coordinates
            ImGui::SameLine(size.x - 280); ImGui::Text("X: %f, Y: %f, Z: %f", cameraEntity->GetWorldPosition().x, cameraEntity->GetWorldPosition().y, cameraEntity->GetWorldPosition().z);
        }

        ImGui::EndMainMenuBar();
    }
}

void Editor::ShowGridSettings() {
    if (showGridSettings) {
        ImGui::SetNextWindowPos(ImVec2(1275, 25));
        ImGui::SetNextWindowSizeConstraints(ImVec2(255, 150), ImVec2(255, 150));
        ImGui::Begin("Grid Settings", &showGridSettings, ImGuiWindowFlags_NoTitleBar);
        ImGui::DragInt("Grid Size", &gridSettings.gridSize, 1.0f, 0, 100);
        EditorSettings::GetInstance().SetLong("Grid Size", gridSettings.gridSize);
        ImGui::DragInt("Line Width", &gridSettings.lineWidth, 0.1f, 1, 5);
        EditorSettings::GetInstance().SetLong("Grid Line Width", gridSettings.lineWidth);
        ImGui::Checkbox("Grid Snap", &gridSettings.gridSnap);
        EditorSettings::GetInstance().SetBool("Grid Snap", gridSettings.gridSnap);
        ImGui::DragInt("Snap Size", &gridSettings.snapOption, 1.0f, 1, 100);
        EditorSettings::GetInstance().SetLong("Grid Snap Size", gridSettings.snapOption);
        if (gridSettings.snapOption < 1)
            gridSettings.snapOption = 1;
        ImGui::End();
    }
}

void Editor::CreateGrid(int size) {
    glm::vec2 gridWidthDepth(10.0f, 10.0f);
    gridWidthDepth.x = (gridWidthDepth.x * size);
    gridWidthDepth.y = (gridWidthDepth.y * size);
    
    float xStart = (-gridWidthDepth.x / 2);
    float xEnd = (gridWidthDepth.x / 2);
    float zStart = (-gridWidthDepth.y / 2);
    float zEnd = (gridWidthDepth.y / 2);
    
    if (size <= 100 && size > 0) {
        for (int i = 0; i < (size + size + 1); i++) {
            Managers().debugDrawingManager->AddLine(glm::vec3(xStart, 0.0f, -gridWidthDepth.y / (2)), glm::vec3(xStart, 0.0f, zEnd), glm::vec3(0.1f, 0.1f, 0.5f), static_cast<float>(gridSettings.lineWidth));
            Managers().debugDrawingManager->AddLine(glm::vec3(-gridWidthDepth.x / (2), 0.0f, zStart), glm::vec3(xEnd, 0.0f, zStart), glm::vec3(0.5f, 0.1f, 0.1f), static_cast<float>(gridSettings.lineWidth));
            xStart += (gridWidthDepth.x / 2) / size;
            zStart += (gridWidthDepth.y / 2) / size;
        }
    }
}

void Editor::ControlEditorCamera(float deltaTime) {
    if (Input()->Pressed(InputHandler::CAMERA)) {
        if (Input()->Triggered(InputHandler::CAMERA)) {
            lastX = Input()->GetCursorX();
            lastY = Input()->GetCursorY();
        }

        float sensitivity = 0.3f;
        cameraEntity->rotation.x += sensitivity * (Input()->GetCursorX() - lastX);
        cameraEntity->rotation.y += sensitivity * (Input()->GetCursorY() - lastY);

        lastX = Input()->GetCursorX();
        lastY = Input()->GetCursorY();

        glm::mat4 orientation = cameraEntity->GetCameraOrientation();
        glm::vec3 backward(orientation[0][2], orientation[1][2], orientation[2][2]);
        glm::vec3 right(orientation[0][0], orientation[1][0], orientation[2][0]);

        // Move speed scaling.
        float speed = 10.0f * deltaTime * (glm::abs(cameraEntity->position.y) / 10.0f);
        float constantSpeed = 10.0f * deltaTime;

        if (cameraEntity->position.y > 10.0f || cameraEntity->position.y < -10.0f) {
            cameraEntity->position += speed * backward * static_cast<float>(Input()->Pressed(InputHandler::BACKWARD) - Input()->Pressed(InputHandler::FORWARD));
            cameraEntity->position += speed * right * static_cast<float>(Input()->Pressed(InputHandler::RIGHT) - Input()->Pressed(InputHandler::LEFT));
        } else {
            cameraEntity->position += constantSpeed * backward * static_cast<float>(Input()->Pressed(InputHandler::BACKWARD) - Input()->Pressed(InputHandler::FORWARD));
            cameraEntity->position += constantSpeed * right * static_cast<float>(Input()->Pressed(InputHandler::RIGHT) - Input()->Pressed(InputHandler::LEFT));
        }
    }
}

void Editor::Picking() {
    if (Input()->Triggered(InputHandler::SELECT) && !ImGui::IsMouseHoveringAnyWindow()) {
        mousePicker.UpdateProjectionMatrix(cameraEntity->GetComponent < Component::Lens>()->GetProjection(glm::vec2(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y)));
        mousePicker.Update();
        float lastDistance = INFINITY;
        int entityIndex = 0;
        int entityAmount = Hymn().world.GetEntities().size();
        for (int i = 0; i < entityAmount; ++i) {
            selectedEntity = Hymn().world.GetEntities().at(i);
            if (selectedEntity->GetComponent<Component::Mesh>() != nullptr) {
                selectedEntity->GetComponent<Component::Mesh>()->SetSelected(false);
                float intersectDistance = 0.0f;
                if (rayIntersector.RayOBBIntersect(cameraEntity->GetWorldPosition(), mousePicker.GetCurrentRay(),
                    selectedEntity->GetComponent<Component::Mesh>()->geometry->GetAxisAlignedBoundingBox(),
                    selectedEntity->GetModelMatrix(), intersectDistance)) {
                    if (intersectDistance < lastDistance) {
                        lastDistance = intersectDistance;
                        entityIndex = i;
                        if (entityAmount - i == 1) {
                            resourceView.GetScene().entityEditor.SetEntity(Hymn().world.GetEntities().at(entityIndex));
                            resourceView.GetScene().entityEditor.SetVisible(true);
                            selectedEntity->GetComponent<Component::Mesh>()->SetSelected(true);
                            break;
                        }
                      
                    } else if (intersectDistance > 0.0f) {
                        resourceView.GetScene().entityEditor.SetEntity(Hymn().world.GetEntities().at(entityIndex));
                        resourceView.GetScene().entityEditor.SetVisible(true);
                        selectedEntity->GetComponent<Component::Mesh>()->SetSelected(true);
                        break;
                    }
                }
            }
        }
    }
}

void Editor::Focus() {
    if (Input()->Triggered(InputHandler::FOCUS)) {
        if (selectedEntity != nullptr) {
            glm::vec3 backward = glm::normalize(cameraEntity->position - selectedEntity->position);
            
            while (glm::length(selectedEntity->position - cameraEntity->position) > 10)
                cameraEntity->position -= backward;
            
            while (glm::length(selectedEntity->position - cameraEntity->position) < 10)
                cameraEntity->position += backward;
            
            glm::vec3 camDirection = selectedEntity->position - cameraEntity->position;
            glm::normalize(camDirection);

            float yaw = std::atan2(camDirection.x, -camDirection.z);
            cameraEntity->rotation.x = glm::degrees(yaw);

            float xz = std::sqrt(camDirection.x * camDirection.x + camDirection.z * camDirection.z);
            float pitch = std::atan2(-camDirection.y, xz);
            cameraEntity->rotation.y = glm::degrees(pitch);
        }
    }
}

void Editor::Play() {

    sceneState = Hymn().world.GetSaveJson();

    SetVisible(false);
    resourceView.HideEditors();

    Managers().scriptManager->RegisterInput();
    Managers().scriptManager->BuildAllScripts();
}

void Editor::LoadSceneState() {
    Hymn().world.Load(sceneState);
}

void Editor::NewHymn() {
    selectHymnWindow.Scan();
    selectHymnWindow.SetClosedCallback(std::bind(&Editor::NewHymnClosed, this, std::placeholders::_1));
    selectHymnWindow.SetTitle("New Hymn");
    selectHymnWindow.SetOpenButtonName("Create");
    selectHymnWindow.SetVisible(true);
}

void Editor::NewHymnClosed(const std::string& hymn) {
    // Create new hymn.
    if (!hymn.empty()) {
        resourceView.ResetScene();
        Hymn().Clear();
        Resources().Clear();
        Hymn().world.CreateRoot();
        Hymn().SetPath(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Hymns" + FileSystem::DELIMITER + hymn);
        resourceView.SetVisible(true);

        // Default scene.
        //Resources().scenes.push_back("Scene #0");

        Entity* player = Hymn().world.GetRoot()->AddChild("Player");
        player->position.z = 10.f;
        player->AddComponent<Component::Lens>();
        player->AddComponent<Component::Listener>();

        Entity* sun = Hymn().world.GetRoot()->AddChild("Sun");
        sun->AddComponent<Component::DirectionalLight>();
    }

    selectHymnWindow.SetVisible(false);

}

void Editor::OpenHymn() {
    selectHymnWindow.Scan();
    selectHymnWindow.SetClosedCallback(std::bind(&Editor::OpenHymnClosed, this, std::placeholders::_1));
    selectHymnWindow.SetTitle("Open Hymn");
    selectHymnWindow.SetOpenButtonName("Open");
    selectHymnWindow.SetVisible(true);
}

void Editor::OpenHymnClosed(const std::string& hymn) {
    // Open hymn.
    if (!hymn.empty()) {
        std::string path = FileSystem::DataPath("Hymn to Beauty") + "/Hymns/" + hymn;
        if (!FileSystem::FileExists((path + "/Hymn.json").c_str()))
            Log() << "Hymn does not exist: " << path << "\n";
        else {
            resourceView.ResetScene();
            Hymn().Load(path);
            Resources().Clear();
            Resources().Load();
            LoadActiveScene();
            resourceView.SetVisible(true);
        }
    }

    selectHymnWindow.SetVisible(false);
}

void Editor::LoadActiveScene() {
    // Load active scene.
    Hymn().world.Load(Hymn().GetPath() + "/" + Resources().activeScene + ".json");
}
