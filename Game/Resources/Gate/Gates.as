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
        @cart = self.GetParent().GetParent().GetChild("Minecart");
        trigger = false;
        speed = 0.0f;
        lastSpeed = 0.0f;
        
        RegisterUpdate();
    }
    
    void Update(float deltaTime){
        self.position.y += speed*deltaTime;
        if (trigger && self.position.y < 15.0f){
            speed = 5.0f;
        } else if (self.position.y >= 15.0f && self.position.y < 25.0f)
            speed -= 0.015f;
        else if (self.position.y >= 25.0f){
            speed -= 0.02f;
            trigger = false;
            SendMessage(cart, 1);
        }
        if (lastSpeed - self.position.y > 0.0f)
            speed = 0;
        lastSpeed = self.position.y;
    }
    
    void ReceiveMessage(int signal){
        if(signal == 1 && !trigger)
            trigger = true;
    }
}