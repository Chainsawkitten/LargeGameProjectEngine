class LeftHandAnim {
    Hub @hub;
    Entity @self;
	Component::AnimationController @animationController;
	
    LeftHandAnim(Entity @entity){
        @hub = Managers();
        @self = @entity;
		@animationController = self.GetAnimationController();
		
        RegisterUpdate();
    }

    void Update(float deltaTime) {
		if (!Input(Trigger, self)) {
			animationController.SetBool("Open", true);
			animationController.SetBool("Closed", false);
        }
		
		if (Input(Trigger, self)) {
			animationController.SetBool("Open", false);
			animationController.SetBool("Closed", true);
        }
    }
}
