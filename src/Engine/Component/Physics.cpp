#include "Physics.hpp"

#include "../Util/Json.hpp"

namespace Component {
    Json::Value Physics::Save() const {
        Json::Value component;
        component["velocity"] = Json::SaveVec3(velocity);
        component["maxVelocity"] = maxVelocity;
        component["angularVelocity"] = Json::SaveVec3(angularVelocity);
        component["maxAngularVelocity"] = maxAngularVelocity;
        component["acceleration"] = Json::SaveVec3(acceleration);
        component["angularAcceleration"] = Json::SaveVec3(angularAcceleration);
        component["velocityDragFactor"] = velocityDragFactor;
        component["angularDragFactor"] = angularDragFactor;
        component["gravityFactor"] = gravityFactor;
        component["momentOfInertia"] = Json::SaveVec3(momentOfInertia);
        return component;
    }
}
