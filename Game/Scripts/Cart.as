class Cart{
    Hub @hub;
    Entity @self;
    float speed;
    bool trigger;
    float stopTime;
    float endTime;
    float a;
    float b;
    float c;
    bool hasHitPlane;
    
    Component::Physics @minecartPhysics;
    Component::Physics @stopPhysics;
    
    Cart(Entity @entity){
        @hub = Managers();
        @self = @entity;
        speed = 20.0f;
        stopTime = 0.0f;
        endTime = 7.5f;
        hasHitPlane = false;
        
        // Calculate second grade equation.
        float t = endTime;
        c = speed;
        a = (3.0f * c - 300.0f / t) / (t * t);
        b = 100.0f / (t * t) - 2.0f * a * t / 3.0f - 2.0f * c / t;
        
        Entity @stopTrigger = self.GetParent().GetChild("GateAndLever").GetChild("StopTrigger");
        @minecartPhysics = self.GetPhysics();
        @stopPhysics = stopTrigger.GetPhysics();
        
        trigger = false;
        RegisterUpdate();
        RegisterTrigger(stopPhysics, minecartPhysics, "OnTrigger");
    }
    
    //Update carts movements and send it's position to Player Script.
    void Update(float deltaTime){
        self.position.z -= speed*deltaTime;
        
        // Braking phase
        if (hasHitPlane && stopTime < endTime){
            stopTime += deltaTime;
            float t = stopTime;
            float zPos = a * t * t * t / 3.0f + b * t * t / 2.0f + c * t;
            self.position.z = 450.0f - zPos;
        }
        // Stopping phase
        else if (stopTime >= endTime && !trigger) {
            self.position.z = 400.0f;
            speed = 0.0f;
        }
        // Start again after lever has been pulled
        else if (trigger){
            if (speed < 20.0f)
                speed += 0.0664f;
        }
    }
    
    void ReceiveMessage(int signal){
        if (signal == 1)
            trigger = true;
    }
    
    void OnTrigger(Component::Physics @trigger, Component::Physics @enterer) {
        print("WOW! WHAT A COLLISION!");
        hasHitPlane = true;
    }
}