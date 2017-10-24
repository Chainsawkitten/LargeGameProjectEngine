#pragma once

#include <glm/vec3.hpp>
#include "../linking.hpp"

class btCollisionShape;
class PhysicsManager;

namespace Physics {
    class Trigger;

    /// Represents a shape for physics objects and facilitates creation of
    /// underlying types.
    class Shape {
        friend class ::PhysicsManager;
        friend class Trigger;

        public:
        /// Parameters used to create a sphere shape.
        struct Sphere {
            Sphere(float radius)
                : radius(radius) {}
            float radius;
        };

        /// Parameters used to create a plane shape.
        struct Plane {
            Plane(const glm::vec3& normal, float planeCoeff)
                : normal(normal), planeCoeff(planeCoeff) {}
            glm::vec3 normal;
            float planeCoeff;
        };

        /// The various kinds of shapes that are wrapped by %Shape.
        enum class Kind {
            Sphere,
            Plane,
        };

        /// Construct a sphere shape.
        /**
             * @param params Sphere specific parameters.
             */
        ENGINE_API Shape(const Sphere& params);

        /// Construct a plane shape.
        /**
             * @param params Plane specific parameters.
             */
        ENGINE_API Shape(const Plane& params);

        /// Get the type of wrapped shape.
        /**
             * @return The type of shape.
             */
        ENGINE_API Kind GetKind() const;

        /// Get sphere data of the shape.
        /**
             * @return Sphere data, or nullptr if the shape is not a sphere.
             */
        ENGINE_API const Sphere* GetSphereData() const;

        /// Get plane data of the shape.
        /**
             * @return Plane data, or nullptr if the shape is not a plane.
             */
        ENGINE_API const Plane* GetPlaneData() const;

        private:
        /// Get the wrapped Bullet shape.
        /**
             * @return The Bullet shape.
             */
        btCollisionShape* GetShape() const;

        Shape(const Shape& other) = delete;

        btCollisionShape* shape;
        Kind kind;
        union {
            Sphere sphere;
            Plane plane;
        };
    };
}
