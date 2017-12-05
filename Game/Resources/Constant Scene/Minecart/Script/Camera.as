class Camera{
    Entity @self;
    vec2 Cursor;
    
    Camera(Entity @entity){
        @self = @entity;
        Cursor = vec2(0,0);
        
        RegisterUpdate();
    }
    
    // Update the camera's rotation based on mouse movement.
    void MouseUpdate(){
        if(Cursor.x == 0 && Cursor.y == 0)
            Cursor = GetCursorXY();

        float horizontal = radians(0.3f * (Cursor.x - GetCursorXY().x));
        float vertical = radians(0.3f * (Cursor.y - GetCursorXY().y));
        self.RotateAroundWorldAxis(horizontal, vec3(0.0f, 1.0f, 0.0f));
        self.RotatePitch(vertical);
        
        Cursor = GetCursorXY();
    }
    
    void Update(float deltaTime) {
        // Only control camera with mouse if we're not running in VR.
        if (!IsVRActive()){
            MouseUpdate();
            //self.RotateYaw(0.02f);
            //self.position.y = -2;
        }
    }
}