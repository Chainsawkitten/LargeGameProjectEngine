class Player {
    Hub @hub;
    Entity @self;
    bool first;

    Player(Entity @entity){
        @hub = Managers();
        @self = @entity;
        first = true;
        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        if (first && IsVRActive() == false)
            self.position.y = 4.0f;
        first = false;
    }
}
