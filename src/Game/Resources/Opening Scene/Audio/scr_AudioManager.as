class scr_AudioManager{
    Hub @hub;
    Entity @self;
    Component::SoundSource @ambient;
    float timer;
    bool triggered;
    
    scr_AudioManager(Entity @entity){
        @hub = Managers();
        @self = @entity;
        RegisterUpdate();
        @ambient = @self.GetSoundSource();
        triggered = false;
    }
    
    void Update(float deltaTime){
        timer += deltaTime;
        if(!triggered && timer > 3)
            ambient.Play();
    }
}