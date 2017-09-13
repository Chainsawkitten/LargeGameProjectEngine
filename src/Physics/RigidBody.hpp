#pragma once

#include <glm/glm.hpp>

class btRigidBody;

namespace Physics {

    class Simulator;

    class RigidBody {
        friend class Simulator;

        public:
            /// Constructor.
            RigidBody();

            /// Destructor.
            ~RigidBody();

            /// Set the mass of a rigid body.
            /**
             * @param mass Mass in kilograms.
             */
            void Mass(float mass);

            /// Get the position of a rigid body.
            /**
             * @return Position of the rigid body.
             */
            glm::vec3 Position();

            /// Set the position of a rigid body.
            /**
             * @param pos The new position.
             */
            void Position(glm::vec3 const& pos);

        private:
            RigidBody(RigidBody& other) = delete;
            void operator=(RigidBody& rhs) = delete;

        private:
            btRigidBody* rigidBody = nullptr;
            Simulator* simulator = nullptr;
    };

}
