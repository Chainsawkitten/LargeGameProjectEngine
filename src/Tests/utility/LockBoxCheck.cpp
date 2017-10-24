#include <memory>
#include <Utility/LockBox.hpp>
#include <catch.hpp>

TEST_CASE("LockBox", "[LockBox]") {
    struct TestData {
        uint32_t foo = 0;
    } data;

    // Create a key and a lockbox with a matching lock.
    auto key = std::shared_ptr<Utility::LockBox<TestData>::Key>(new Utility::LockBox<TestData>::Key());
    auto box = Utility::LockBox<TestData>(key, &data);

    SECTION("Initial reference count") {
        REQUIRE(box.RefCount() == 1);
    }

    SECTION("Opening a box with the correct key") {
        bool funcCalled = false;
        REQUIRE(box.Open(key, [&funcCalled](TestData& t) {
            funcCalled = true;
        }));
        REQUIRE(funcCalled);
    }

    SECTION("Opening a box with the wrong key") {
        auto wrongKey = std::shared_ptr<Utility::LockBox<TestData>::Key>(new Utility::LockBox<TestData>::Key());
        REQUIRE_FALSE(box.Open(wrongKey, [](TestData& t) {
            // Return value is one thing, but we must also make sure that this
            // function is never called.
            REQUIRE(false);
        }));
    }

    SECTION("Manipulate lockbox contents") {
        box.Open(key, [](TestData& t) {
            t.foo++;
        });
        REQUIRE(data.foo == 1);
    }

    SECTION("Copied box") {
        Utility::LockBox<TestData> copy(box);

        SECTION("Copied box increases reference count") {
            REQUIRE(box.RefCount() == 2);
            REQUIRE(copy.RefCount() == 2);
        }

        SECTION("Copied box can be opened by the original key") {
            bool funcCalled = false;
            REQUIRE(copy.Open(key, [&funcCalled](TestData& t) {
                funcCalled = true;
            }));
            REQUIRE(funcCalled);
        }

        SECTION("Copied box manipulates same data as original box") {
            REQUIRE(data.foo == 0);
            box.Open(key, [](TestData& t) {
                t.foo++;
            });
            REQUIRE(data.foo == 1);
            copy.Open(key, [](TestData& t) {
                t.foo++;
            });
            REQUIRE(data.foo == 2);
        }
    }

    SECTION("Assignment") {
        auto assigned = box;

        SECTION("Assigned box increases reference count") {
            REQUIRE(box.RefCount() == 2);
            REQUIRE(assigned.RefCount() == 2);

            SECTION("Self assignment keeps reference count") {
                box = box;
                REQUIRE(box.RefCount() == 2);
            }
        }

        SECTION("Assigned box can be opened by the original key") {
            bool funcCalled = false;
            REQUIRE(assigned.Open(key, [&funcCalled](TestData& t) {
                funcCalled = true;
            }));
            REQUIRE(funcCalled);
        }

        SECTION("Assigned box manipulates same data as original box") {
            REQUIRE(data.foo == 0);
            box.Open(key, [](TestData& t) {
                t.foo++;
            });
            REQUIRE(data.foo == 1);
            assigned.Open(key, [](TestData& t) {
                t.foo++;
            });
            REQUIRE(data.foo == 2);
        }

        SECTION("Move assignment keeps reference count and invalidates the moved box") {
            auto moved = std::move(assigned);
            REQUIRE(box.RefCount() == 2);
            REQUIRE(moved.RefCount() == 2);
            REQUIRE(assigned.RefCount() == 0);
        }
    }

    SECTION("Destructor decreases reference count") {
        {
            auto temp = box;
            REQUIRE(box.RefCount() == 2);
        }
        REQUIRE(box.RefCount() == 1);
    }

    SECTION("Opening default created box does nothing") {
        Utility::LockBox<TestData> box;
        REQUIRE_NOTHROW(box.Open(key, [](TestData& t) {
            REQUIRE(false);
        }));
    }
}
