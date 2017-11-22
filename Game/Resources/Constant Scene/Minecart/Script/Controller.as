class Controller {
    Hub @hub;
    Entity @self;
    Entity @rock;
    Entity @cart;
    bool isPressed;

    Controller(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @rock = GetEntityByGUID(1510240479);
        @cart = GetEntityByGUID(1508919384);
        isPressed = false;

        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        //if (self.name == "Right Controller") {
        //    print("Controller position: (" + self.GetWorldPosition().x + ", " + self.GetWorldPosition().y + ", " + self.GetWorldPosition().z + ")\n");
        //}
        if(!IsVRActive()){
            self.position.y = -3;
            self.position.x = -1;
        }
        if (!Input(Trigger) && isPressed) {
            isPressed = false;
            SendMessage(rock, 2);
        }
    }
    
    void OnRockTrigger() {
        //print("Rock trigger\n");
        if (Input(Trigger) && !isPressed) {
            isPressed = true;
            SendMessage(rock, 1);
        } 
    }
}
