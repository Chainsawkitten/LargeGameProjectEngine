class RightHandAnim {
    Hub @hub;
    Entity @self;
		Component::AnimationController @animationController;

    RightHandAnim(Entity @entity){
        @hub = Managers();
        @self = @entity;
		@animationController = self.GetAnimationController();

        RegisterUpdate();
    }

    void Update(float deltaTime) {

    }
}
