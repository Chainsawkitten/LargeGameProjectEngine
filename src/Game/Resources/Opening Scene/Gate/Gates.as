class Gates{
    Hub @hub;
    Entity @self;
    Entity @cart;
    bool trigger;
    float speed;
    float lastSpeed;
    
    Gates(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @cart = self.GetParent().GetChild("Minecart").GetChild("Minecart");
        trigger = false;
        speed = 0.0f;
        lastSpeed = 0.0f;
        
        RegisterUpdate();
    }
    
    void Update(float deltaTime){
        self.position.y += speed*deltaTime;
        if (lastSpeed - self.position.y > 0.0f)
            speed = 0.0f;
        else if (trigger && self.position.y < -1.0f)
            speed = 0.2f;
        else if (self.position.y >= -1.0f && self.position.y < 0.0f)
            speed -= 0.00015f;
        else if (self.position.y >= 0.5f){
            speed -= 0.002f;
            trigger = false;
            SendMessage(cart, 1);
        }
        lastSpeed = self.position.y;
    }
    
    void ReceiveMessage(int signal){
        if(signal == 1 && !trigger)
            trigger = true;
    }
}