class Cart{
    Hub @hub;
    Entity @self;
    Cart(Entity @entity){
        @hub = Managers();
        @self = @entity;
        RegisterUpdate();
    }
    
    //Update carts movements and send it's position to Player Script.
    void Update(float deltaTime){
        self.position.z -= 20.0f*deltaTime;
    }
}