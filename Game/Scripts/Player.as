class Player{
    Hub @hub;
    Entity @self;
    vec2 Cursor;
    Player(Entity @entity){
        @hub = Managers();
        @self = @entity;
        RegisterUpdate();
        Cursor = vec2(0,0);
    }
    
    void Update(float i){
        if(Cursor.x == 0 && Cursor.y == 0)
            Cursor = GetCursorXY();
        self.rotation.x += 0.3f * (GetCursorXY().x - Cursor.x);
        self.rotation.y += 0.3f * (GetCursorXY().y - Cursor.y);
        
        Cursor = GetCursorXY();
    }
}