class global_music_script {
    Hub @hub;
    Entity @self;
    
    //Sound components.
    Component::SoundSource @snd_mystic;
    Component::SoundSource @snd_hunted;
    
    Component::SoundSource @snd_charged;
    Component::SoundSource @snd_waterphone;
    
    Component::SoundSource @snd_horn;
    Component::SoundSource @snd_gameover;
    Component::SoundSource @snd_win;

    global_music_script(Entity @entity){
        @hub = Managers();
        @self = @entity;
        //cart 1509959164
        //Retrieve all music files.
        @snd_mystic=GetEntityByGUID(1512735147).GetSoundSource();
        @snd_hunted=GetEntityByGUID(1512735129).GetSoundSource();
        @snd_waterphone=GetEntityByGUID(1512735100).GetSoundSource();
        
        @snd_gameover=GetEntityByGUID(1512735079).GetSoundSource();
        @snd_charged=GetEntityByGUID(1512735049).GetSoundSource();
        
        @snd_horn=GetEntityByGUID(1512735015).GetSoundSource();
        @snd_win=GetEntityByGUID(1512735032).GetSoundSource();

    }
    
    void PlayMysticString(){
        snd_mystic.Play();
    }
    void PlayHunted(){
        snd_hunted.Play();
    }
    void PlayCharged(){
        snd_charged.Play();
    }
    void PlayWaterphone(){
        snd_waterphone.Play();
    }
    void PlayHorn(){
        snd_horn.Play();
    }
    void PlayGameOver(){
        snd_gameover.Play();
    }
    void PlayWin(){
        snd_win.Play();
    }
    
    void FadeHunted(){
        snd_hunted.Stop();
    }
}
