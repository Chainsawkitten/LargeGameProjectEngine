#include "NodeEditor.hpp"

#include <Engine/Animation/AnimationController.hpp>
#include <Utility/Log.hpp>

NodeEditor::NodeEditor() {
}

NodeEditor::~NodeEditor() {
}

void NodeEditor::Show() {
    if (ImGui::Begin("Animation controller", &visible)) {
        // Draw a list of nodes on the left side
        ImGui::BeginChild("NodeList", ImVec2(200, 0), true);
        ImGui::Text("Nodes");
        ImGui::Separator();
        for (int nodeId = 0; nodeId < GetNumNodes(); ++nodeId) {
            Node* node = GetNodeArray()[nodeId];
            ImGui::PushID(node->index);
            if (ImGui::Selectable(node->name, node->index == nodeSelected))
                nodeSelected = node->index;
            if (ImGui::IsItemHovered()) {
                nodeHoveredInList = node->index;
                openContextMenu |= ImGui::IsMouseClicked(1);
            }
            ImGui::PopID();
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginGroup();

        // Grid.
        ShowGrid();

        ImVec2 offset = ImVec2(ImGui::GetCursorScreenPos().x - scrolling.x, ImGui::GetCursorScreenPos().y - scrolling.y);
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->ChannelsSplit(2);

        ImU32 gridColorRight = ImColor(150, 150, 150, 200);
        ImU32 gridColorUp = ImColor(110, 110, 110, 200);
        float gridSquareSize = 64.0f;
        ImVec2 winPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetWindowSize();
        for (float x = fmodf(offset.x, gridSquareSize); x < canvasSize.x; x += gridSquareSize)
            drawList->AddLine(ImVec2(x, winPos.y), ImVec2(x, canvasSize.y + winPos.y), gridColorRight);
        for (float y = fmodf(offset.y, gridSquareSize); y < canvasSize.y; y += gridSquareSize)
            drawList->AddLine(ImVec2(winPos.x, y), ImVec2(canvasSize.x + winPos.x, y), gridColorUp);

        // Draw connection.
        if (isDragingConnection) {
            Node * node = GetNodeArray()[dragNodeIndex];
            ImVec2 outPos = ImVec2(node->pos.x + offset.x + node->size.x, node->pos.y + offset.y + (node->size.y / 2));
            ImVec2 p2 = ImGui::GetIO().MousePos;
            ImVec2 p2Offset = ImVec2(p2.x - 100.0f, p2.y);
            drawList->AddBezierCurve(outPos, ImVec2(outPos.x + 100.0f, outPos.y), p2Offset, p2, ImColor(40, 40, 40), 2.0f);
        }

        for (unsigned int id = 0; id < GetNumNodes(); ++id) {
            Node * action = GetNodeArray()[id];

            for (unsigned int connections = 0; connections < action->numOutputSlots; ++connections) {
                ImVec2 p1 = ImVec2(action->pos.x + offset.x + action->size.x, action->pos.y + offset.y + (action->size.y / 2));
                ImVec2 p1Offset = ImVec2(p1.x + 100.0f, p1.y);
                Node * connectionNode = GetNodeArray()[action->outputIndex[connections]];
                ImVec2 p2 = ImVec2(connectionNode->pos.x + offset.x, connectionNode->pos.y + offset.y + (connectionNode->size.y / 2));
                ImVec2 p2Offset = ImVec2(p2.x - 100.0f, p2.y);
                drawList->AddBezierCurve(p1, p1Offset, p2Offset, p2, ImColor(80, 80, 80), 2.0f);
            }
        }

        hoveredNodeIndex = -1;

        // Draw animation actions.
        for (unsigned int id = 0; id < GetNumNodes(); ++id) {
            Node * node = GetNodeArray()[id];
            ImGui::PushID(id);
            ImVec2 node_rect_min = ImVec2(offset.x + node->pos.x, offset.y + node->pos.y);

            drawList->ChannelsSetCurrent(1);
            bool oldActive = ImGui::IsAnyItemActive();
            ImGui::SetCursorScreenPos(ImVec2(node_rect_min.x + nodeWindowPadding.x, node_rect_min.y + nodeWindowPadding.y));
            ImGui::BeginGroup();
            ShowNode(node);
            ImGui::EndGroup();

            // Save the size of what we have emitted and whether any of the widgets are being used.
            bool node_widgets_active = (!oldActive && ImGui::IsAnyItemActive());
            node->size = glm::vec2(ImGui::GetItemRectSize().x + nodeWindowPadding.x + nodeWindowPadding.x, ImGui::GetItemRectSize().y + nodeWindowPadding.y + nodeWindowPadding.y);
            ImVec2 nodeRectMax = ImVec2(node_rect_min.x + node->size.x, node_rect_min.y + node->size.y);

            // Display node box
            drawList->ChannelsSetCurrent(0);
            ImGui::SetCursorScreenPos(node_rect_min);
            ImGui::InvisibleButton("Node", ImVec2(node->size.x, node->size.y));
            if (ImGui::IsItemHovered()) {
                nodeHoveredInScene = node->index;
                openContextMenu |= ImGui::IsMouseClicked(1);
            }

            bool nodeMovingActive = ImGui::IsItemActive();

            if (node_widgets_active || nodeMovingActive)
                nodeSelected = node->index;

            if (nodeMovingActive && ImGui::IsMouseDragging(0))
                node->pos += glm::vec2(ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y);

            ImU32 nodeBackground = (nodeHoveredInList == node->index || nodeHoveredInScene == node->index || (nodeHoveredInList == -1 && nodeSelected == node->index)) ? ImColor(220, 220, 220) : ImColor(190, 190, 190);
            drawList->AddRectFilled(node_rect_min, nodeRectMax, nodeBackground, 4.0f);
            drawList->AddRect(node_rect_min, nodeRectMax, ImColor(100, 100, 100), 4.0f);

            ImVec2 outPos = ImVec2(node->pos.x + offset.x + node->size.x, node->pos.y + offset.y + (node->size.y / 2));
            ImVec2 inPos = ImVec2(node->pos.x + offset.x, node->pos.y + offset.y + (node->size.y / 2));
            ImColor inColor = ImColor(70, 70, 256, 256);
            ImColor outColor = ImColor(70, 256, 70, 256);

            if (ImGui::GetIO().MousePos.x < outPos.x + nodeSlotRadius && ImGui::GetIO().MousePos.x > outPos.x - nodeSlotRadius && ImGui::GetIO().MousePos.y < outPos.y + nodeSlotRadius && ImGui::GetIO().MousePos.y > outPos.y - nodeSlotRadius) {
                hoveredNodeIndex = id;
                outColor = ImColor(100, 256, 100, 256);
            } else if (ImGui::GetIO().MousePos.x < inPos.x + nodeSlotRadius && ImGui::GetIO().MousePos.x > inPos.x - nodeSlotRadius && ImGui::GetIO().MousePos.y < inPos.y + nodeSlotRadius && ImGui::GetIO().MousePos.y > inPos.y - nodeSlotRadius) {
                hoveredNodeIndex = id;
                inColor = ImColor(100, 100, 256, 256);
            }

            drawList->AddCircleFilled(outPos, nodeSlotRadius, outColor, 24);
            drawList->AddCircleFilled(inPos, nodeSlotRadius, inColor, 24);

            ImGui::PopID();
        }

        drawList->ChannelsMerge();

        if (ImGui::GetIO().MouseClicked[0]) {
            if (hoveredNodeIndex == -1)
                isDragingConnection = false;
            else {
                if (isDragingConnection) {
                    if (GetNodeArray()[dragNodeIndex]->numOutputSlots < 8) {
                        if (CanConnect(GetNodeArray()[dragNodeIndex], GetNodeArray()[hoveredNodeIndex])) {
                            GetNodeArray()[dragNodeIndex]->outputIndex[GetNodeArray()[dragNodeIndex]->numOutputSlots] = hoveredNodeIndex;
                            GetNodeArray()[dragNodeIndex]->numOutputSlots += 1;
                        }
                    }
                    isDragingConnection = false;
                } else {
                    dragNodeIndex = hoveredNodeIndex;
                    isDragingConnection = true;
                }
            }
        }

        if (ImGui::BeginPopupContextWindow("AnimCtrlContextPopup")) {
            ShowContextMenu();
            ImGui::EndPopup();
        }

        // Scrolling
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
            scrolling = ImVec2(scrolling.x - ImGui::GetIO().MouseDelta.x, scrolling.y - ImGui::GetIO().MouseDelta.y);

        ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        ImGui::EndGroup();
    }
    ImGui::End();
}

bool NodeEditor::IsVisible() const {
    return visible;
}

void NodeEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void NodeEditor::ShowGrid() {
    ImGui::Text("Graph editor");
    ImGui::SameLine(ImGui::GetWindowWidth() - 100);
    ImGui::Checkbox("Show grid", &showGrid);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(60, 60, 70, 200));
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
    ImGui::PushItemWidth(120.0f);
}
