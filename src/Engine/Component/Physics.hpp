#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

class PhysicsManager;

namespace Component {
    /// Will be removed soon™.
    class Physics : public SuperComponent {
        friend class ::PhysicsManager;
        
        public:
            /// Create new physics component.
            Physics() = default;
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Velocity (in meter/second).
            glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);

            /// Max velocity (in meter/second).
            float maxVelocity = 20.f;

            /// Angular velocity (in Hz).
            glm::vec3 angularVelocity = glm::vec3(0.f, 0.f, 0.f);

            /// Max angular velocity (in Hz).
            float maxAngularVelocity = 2.f;

            /// Acceleration (in meter/second^2).
            glm::vec3 acceleration = glm::vec3(0.f, 0.f, 0.f);

            /// Angular acceleration (in Hz/second).
            glm::vec3 angularAcceleration = glm::vec3(0.f, 0.f, 0.f);

            /// Friction factor. Describes the percentage of the velocity that is lost each second. 
            float velocityDragFactor = 1.f;

            /// Angular drag factor. Describes the percentage of the rotation that is lost each second. 
            float angularDragFactor = 1.f;

            /// Gravity factor. 
            float gravityFactor = 0.f;

            /// Moment of inertia. Describes how hard it's to rotate an object.
            /**
             * https://en.wikipedia.org/wiki/List_of_moments_of_inertia
             */
            glm::vec3 momentOfInertia = glm::vec3(1.f, 1.f, 1.f);
    };
}
