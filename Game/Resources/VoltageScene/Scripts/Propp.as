class Propp {
    Hub @hub;
    Entity @self;
    Entity @rightCtrl;
    Entity @originalParent;
    Entity @S1T1;
    bool isPressed;

    Propp(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @rightCtrl = GetEntityByGUID(1508919758);
        @originalParent = self.GetParent();
        @S1T1 = GetEntityByGUID(1905121);
        
        isPressed = false;
        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        if(Input(OpenGate, self)) {
            self.position = vec3(2.422f, 6.817f, -2.762f);
        }
        /*if (!Input(Trigger, rightCtrl) && isPressed) {
            isPressed = false;
            self.SetParent(originalParent);
        }*/
    }
    
   /* void PickupTrigger() {
        if (Input(Trigger, rightCtrl) && !isPressed) {
            isPressed = true;
            self.position = (0.0f, 0.0f, 0.0f);
            self.SetParent(rightCtrl);
        }
    }*/
}
