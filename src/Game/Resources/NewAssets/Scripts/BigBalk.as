class BigBalk {
    Hub @hub;
    Entity @self;
    Entity @bucketChain;
    Entity @stoneChain;
    Entity @bridgeParent;
    bool rotateCheck;
    float rotateLimit;

    BigBalk(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @bucketChain = GetEntityByGUID(1510911321);
        @stoneChain = GetEntityByGUID(1510911538);
        @bridgeParent = GetEntityByGUID(1510240835);
        rotateCheck = true;
        rotateLimit = pitch(self.rotation);
        
        RegisterUpdate();
    }
    
    void Update(float deltaTime) {
        print("" + degrees(pitch(self.rotation)) + "\n");
        if (pitch(self.rotation) < rotateLimit) {
            float amountOfRotation = radians(2.5f) * deltaTime;
            self.RotatePitch(amountOfRotation);
            bucketChain.RotatePitch(-amountOfRotation);
            stoneChain.RotatePitch(-amountOfRotation);
        }
        if (pitch(self.rotation) >= radians(0.0f))
            SendMessage(bridgeParent, 1);
    }
    
    void ReceiveMessage(Entity @sender, int i) {
        if (i == 1 && rotateCheck == true){
            rotateCheck = false;
            rotateLimit = pitch(self.rotation) + radians(5.0f);
        }
        else if (i == 2 && rotateCheck == false)
            rotateCheck = true;
    }
    
}
