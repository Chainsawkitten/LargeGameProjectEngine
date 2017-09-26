#include <catch.hpp>
#include <Engine/Entity/Entity.hpp>

TEST_CASE("", "[entity component]")
{
    // Create testEntity with null world.
    Entity nullWorldEntity(nullptr, "TestEntity");

    SECTION ("Test default values.")
    {
        REQUIRE(nullWorldEntity.position == glm::vec3(0, 0, 0));
        REQUIRE(nullWorldEntity.rotation == glm::vec3(0, 0, 0));
        REQUIRE(nullWorldEntity.scale == glm::vec3(1, 1, 1));
    }
}
