#include "ConeShapeEditor.hpp"

#include <Engine/Component/Shape.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Physics/Shape.hpp>
#include <imgui.h>

#ifdef USINGMEMTRACK
#include <MemTrack.hpp>
#endif

namespace GUI {
    void ConeShapeEditor::Show(Component::Shape* comp) {
        ImGui::Indent();

        if (ImGui::InputFloat("Radius", &radius))
            Apply(comp);

        if (ImGui::InputFloat("Height", &height))
            Apply(comp);

        ImGui::Unindent();
    }

    void ConeShapeEditor::Apply(Component::Shape* comp) {
        Managers().physicsManager->SetShape(comp, std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Cone(radius, height))));
    }

    bool ConeShapeEditor::SetFromShape(const Physics::Shape& shape) {
        if (shape.GetKind() == Physics::Shape::Kind::Cone) {
            auto coneData = shape.GetConeData();
            radius = coneData->radius;
            height = coneData->height;
            return true;
        }

        return false;
    }
}
