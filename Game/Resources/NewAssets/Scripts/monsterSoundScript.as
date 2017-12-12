class monsterSoundScript {
    Hub @hub;
    Entity @self;
    Entity @monster;
    Entity @footStep;

    monsterSoundScript(Entity @entity){
        @hub = Managers();
        @self = @entity;
        @footStep = GetEntityByGUID(1512650688);
        self.GetSoundSource().Play();
        // Remove this if updates are not desired.
        
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        if(self.GetParent().GetWorldPosition().x < -141.7f)
            footStep.GetSoundSource().Stop();
    }
}
