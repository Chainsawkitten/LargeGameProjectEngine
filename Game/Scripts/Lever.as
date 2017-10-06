class Lever {
    Hub @hub;
    Entity @self;
    Entity @sibling;
    Entity @player;
    
    Lever(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @sibling = self.GetParent().GetChild("Gates");
        @player = self.GetParent().GetParent().GetChild("Minecart").GetChild("Player");
        
        RegisterUpdate();
    }
    
    void Update(float deltaTime){
        if (Input(MouseOpen)){
            if(IsIntersect(self, player)){
                if(self.rotation.y <= -45.0f){
                    SendMessage(sibling, 1);
                }
                else
                    self.rotation.y -= 0.3;
            }
        }
    }
}