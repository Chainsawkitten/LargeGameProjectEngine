class Lever {
    Hub @hub;
    Entity @self;
    Entity @sibling;
    Entity @player;
    
    Lever(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @sibling = self.GetParent().GetParent().GetChild("Gates");
        @player = self.GetParent().GetParent().GetChild("Minecart").GetChild("Player");
        
        RegisterUpdate();
    }
    
    void Update(float deltaTime){
        if (Input(Trigger)){
            if (pitch(self.rotation) <= radians(-30.0f)){
                SendMessage(sibling, 1);
            }
            else {
                self.RotatePitch(radians(-1.0f));
            }
        }
    }
}