#pragma once

#include <glm/glm.hpp>
#include "SuperComponent.hpp"

class btRigidBody;
class PhysicsManager;

namespace Physics {
    class Trigger;
}

namespace Component {
    /// %Component that allows interacting with other physics components.
    class RigidBody : public SuperComponent {
        friend class ::PhysicsManager;
        friend class ::Physics::Trigger;

        public:
            /// Constructor.
            RigidBody() = default;

            /// Destructor.
            ~RigidBody();

            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;

        private:
            // Get the underlying Bullet rigid body. If none has been set,
            // nullptr is returned.
            btRigidBody* GetBulletRigidBody();

            // Creates the underlying Bullet rigid body. Mass is provided in
            // units of kilograms.
            void NewBulletRigidBody(float mass);

            // Destroy resources completely.
            void Destroy();

            // Get the position of a rigid body.
            glm::vec3 Position() const;

            // Set the position of a rigid body.
            void Position(const glm::vec3& pos);

            // Set the mass in kilograms of a rigid body.
            void Mass(float mass);

        private:
            float mass = 1.0f;
            btRigidBody* rigidBody = nullptr;
    };
}
