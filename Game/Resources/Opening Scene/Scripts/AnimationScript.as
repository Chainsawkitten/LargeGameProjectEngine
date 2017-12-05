class AnimationScript {
    Hub @hub;
    Entity @self;
	Component::AnimationController @animCtrl;
    AnimationScript(Entity @entity){
        @hub = Managers();
        @self = @entity;
		@animCtrl = self.GetAnimationController();
		animCtrl.SetBool("D", false);
        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
    }
	
	void TriggerAnim(){
		animCtrl.SetBool("D", true);
	}
}
