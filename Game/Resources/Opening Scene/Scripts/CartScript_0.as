class CartScript_0 {
    Hub @hub;
    Entity @self;
    //Entity @leverHandle;
    Entity @rightHand;
    float speed;
    vec3 tempPos;


    CartScript_0(Entity @entity){
        @hub = Managers();
        @self = GetEntityByGUID(1508919384);
        @rightHand = GetEntityByGUID(1508919758);
        //@leverHandle=GetEntityByGUID(1512032656);
        
        speed = 2.0f;
     
        // Remove this if updates are not desired.
        RegisterUpdate();
    }
 //  void PullLever(){
 //      if(Input(Trigger, rightHand))
 //          leverHandle.RotatePitch(0.01f);
 //  }

    // Called by the engine for each frame.
    void Update(float deltaTime) {


    
    //if(degrees(pitch(leverHandle.rotation))>120.0f){
        tempPos = self.GetWorldPosition();
        tempPos.x -= speed * deltaTime;
        self.SetWorldPosition(tempPos);
     //}   
    }
}
