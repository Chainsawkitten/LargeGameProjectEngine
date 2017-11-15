class Minecart{
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
    
    Minecart(Entity @entity){
        @hub = Managers();
        @self = @entity;
        speed = 0.0f;
        stopTime = 0.0f;
        endTime = 7.5f;
        hasHitPlane = false;
        
        // Calculate second grade equation.
        float t = endTime;
        c = speed;
        a = (3.0f * c - 300.0f / t) / (t * t);
        b = 100.0f / (t * t) - 2.0f * a * t / 3.0f - 2.0f * c / t;
        
        trigger = false;
        RegisterUpdate();
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
        if (trigger){
            if (speed < 2.0f)
                speed += 0.00664f;
            if (speed > 2.0f){
                speed = 2.0f;
                trigger = false;
            }
        }
    }
    
    void ReceiveMessage(int signal){
        if (signal == 1)
            trigger = true;
    }
}