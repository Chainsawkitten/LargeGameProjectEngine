class Bridge {
    Entity @self;
    Entity @cart;
    Entity @monster;
    bool trigger;
    bool playingAudio;
    bool done;

    Bridge(Entity @entity){
        @self = @entity;
        @cart = GetEntityByGUID(1510920517);
        @monster = GetEntityByGUID(1512394369);
        trigger = false;
        playingAudio = false;
        done = false;
        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        if (trigger == true && roll(self.rotation) < radians(0.0f)){
            if(playingAudio == false){
                playingAudio = true;
                self.GetSoundSource().Play();
            }
            // Speed: 90 degrees over 8 seconds
            self.RotateRoll(radians(90.0f) / 8.0f * deltaTime);
        }
        else if (!done && roll(self.rotation) > 0.0f) {
            trigger = false;
            SendMessage(cart, 3);
            monster.SetEnabled(true, true);
            SendMessage(monster, 0);
            done = true;
        }
    }
    
    void ReceiveMessage(Entity @sender, int i){
        if (i == 1 && trigger == false)
            trigger = true;
    }
}
