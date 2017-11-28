class elevator_frontDoorCon {
    Hub @hub;
    Entity @doorWithX;
	Entity @doorWithService;
	bool openDoor;
	vec3 tempPosWithX;
	vec3 tempPosWithService;
	float speed;
	

    elevator_frontDoorCon(Entity @entity){
        @hub = Managers();
        @doorWithX = GetEntityByGUID(1511870049);
		@doorWithService = GetEntityByGUID(1511870278);
		openDoor = false;
		speed = 0.5f;
        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
		if(openDoor == true) {
			if(tempPosWithX.z < 2.0f) {
				tempPosWithX = doorWithX.GetWorldPosition();
				tempPosWithService = doorWithService.GetWorldPosition();
			
				tempPosWithX.z += speed * deltaTime;
				tempPosWithService.z -= speed * deltaTime;
				
				doorWithX.SetWorldPosition(tempPosWithX);
				doorWithService.SetWorldPosition(tempPosWithService);
			}
		}
		
		if(openDoor == false) {
			if(tempPosWithX.z > 0.0f) {
				tempPosWithX = doorWithX.GetWorldPosition();
				tempPosWithService = doorWithService.GetWorldPosition();
			
				tempPosWithX.z -= speed * deltaTime;
				tempPosWithService.z += speed * deltaTime;
				
				doorWithX.SetWorldPosition(tempPosWithX);
				doorWithService.SetWorldPosition(tempPosWithService);
			}
		}
    }
	
	void OpenDoor() {
		openDoor = true;
	}
	
	void CloseDoor() {
		openDoor = false;
	}
}