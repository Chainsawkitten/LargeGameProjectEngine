#include "../Physics/Shape.hpp"
#include "../Util/Json.hpp"
#include "Shape.hpp"

namespace Component {
    Json::Value Shape::Save() const {
        Json::Value component;

        Json::Value concreteShape;
        switch (shape->GetKind()) {
            case ::Physics::Shape::Kind::Sphere: {
                auto sphereData = shape->GetSphereData();
                concreteShape["radius"] = sphereData->radius;
                component["sphere"] = concreteShape;
                break;
            }
            case ::Physics::Shape::Kind::Plane: {
                auto planeData = shape->GetPlaneData();
                concreteShape["normal"] = Json::SaveVec3(planeData->normal);
                concreteShape["planeCoeff"] = planeData->planeCoeff;
                component["plane"] = concreteShape;
                break;
            }
        }

        return component;
    }

    ::Physics::Shape& Shape::GetShape() {
        return *shape;
    }

    const ::Physics::Shape& Shape::GetShape() const {
        return *shape;
    }

    void Shape::SetShape(::Physics::Shape* shape) {
        delete this->shape;
        this->shape = shape;
    }
}
