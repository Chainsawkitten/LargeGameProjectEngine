class MonsterRun {
    Hub @hub;
    Entity @self;
    vec3 monsterPos;
    float speed;
    bool run;
    Component::AnimationController @animCtrl;
    Component::SoundSource @doorbang;
    
    MonsterRun(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @doorbang = GetEntityByGUID(1512996068).GetSoundSource();
        speed = 2.3f;
        run = false;
        @animCtrl = self.GetAnimationController();
        animCtrl.SetBool("E", false);
        // Remove this if updates are not desired.
        RegisterUpdate();
        self.SetEnabled(false, true);
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        
        if(run){
        
            if(monsterPos.x > -145.7f){
                monsterPos = self.GetWorldPosition();
            
                monsterPos.x -= speed * deltaTime;
        
                self.SetWorldPosition(monsterPos);
            }
            if(monsterPos.x < -144.7f){
                animCtrl.SetBool("E", true);
                doorbang.Play();
                run = false;
            }

        }

    }
    
    void MonsterRunning(){
        run = true;
    }
    
    void ReceiveMessage(Entity @sender, int signal){
        if(signal == 0){
            MonsterRunning();
        }
    }
    
}
