class elevator_moveCart {
    Hub @hub;
    Entity @self;
    Entity @elevator;
    Entity @board;
    Entity @puzzleBoard;
    Entity @realSelf;
    Entity @frontDoor;
    Entity @sndDoorStop;
    
    Entity @cage;
    float speed;
    vec3 tempPos;
    vec3 elevatorPos;
    bool moveForward;
    bool moveUpward;
    int phase;
    float uniformScale;
    bool puzzleSkipped;

    float startStopPos;
    float stopTime;
    float endTime;
    float a;
    float b;
    float c;
    float brakeDistance;
    
    elevator_moveCart(Entity @entity){
        @hub = Managers();
        @self = GetEntityByGUID(1508919163);
        @elevator = GetEntityByGUID(1511870044);
        @board = GetEntityByGUID(1511530025);
        @puzzleBoard = GetEntityByGUID(1512029307);
        @frontDoor = GetEntityByGUID(1511946590);
        @cage = GetEntityByGUID(1511870172);
        @sndDoorStop = GetEntityByGUID(1512740447);

        @realSelf = @entity;
        realSelf.SetEnabled(false, true);
        
        moveForward = true;
        moveUpward = false;
        speed = 2.0f;
        phase = 0;
        uniformScale = 1.0f;
        endTime = 6.5f;

        puzzleSkipped = false;
        // Remove this if updates are not desired.
        RegisterUpdate();
    }

    void ReceiveMessage(Entity @sender, int message) {
        if(message == 0) {
            phase = 1;
        }
    }

    // Called by the engine for each frame.
    void Update(float deltaTime) {
        switch(phase) {
            case 0:
                tempPos = self.GetWorldPosition();
                tempPos.x -= speed * deltaTime;
                self.SetWorldPosition(tempPos);
                break;

            case 1: 
                uniformScale -= (1.0f / 3.5f) * deltaTime;
                if (uniformScale < 0.0f) {
                    uniformScale = 0.0f;
                    phase = 2;
                    cage.GetSoundSource().Play();
                }

                puzzleBoard.scale = vec3(uniformScale, uniformScale, uniformScale);
                break;

            case 2:
                GetEntityByGUID(1512996068).GetSoundSource().Stop(); //Stop door banging sound

                elevatorPos = elevator.GetWorldPosition();
                elevatorPos.y += speed * deltaTime;
                elevator.SetWorldPosition(elevatorPos);

                tempPos = self.GetWorldPosition();
                tempPos.y += speed * deltaTime;
                self.SetWorldPosition(tempPos);
                break;
                
            case 3:
                if (stopTime < endTime){
                    stopTime += deltaTime;
                    tempPos = self.GetWorldPosition();
                    float s = a * stopTime * stopTime * stopTime / 3 + b * stopTime * stopTime / 2 + c * stopTime;
                    tempPos.x = startStopPos - s;
                    self.SetWorldPosition(tempPos);
                } else {
                    
                    //When we've stopped we close the door behind us.
                    SendMessage(frontDoor, 0);
                    phase = 4;
                    cage.GetSoundSource().Stop();
                    sndDoorStop.GetSoundSource().Play();
                    SendMessage(board, 0);

                }
                break;

            case 4: // Cart is stopped.
                // Skip Puzzle.
                if(!IsVRActive() && Input(PuzzleSkip, @self) && !puzzleSkipped) {
                    puzzleSkipped = true;
                    phase = 1;
                    SendMessage(board, 1);
                }
                break;

            case 5: { // Waiting for doors to open
                break;
            }
        }
    }

    void MoveForward() {
        phase = 0;
    }
    
    void SlowDown(){
        startStopPos = self.GetWorldPosition().x;
        brakeDistance = 13.37f; // The distance during which we will brake
        c = speed;
        b = (6 * brakeDistance - 4 * c * endTime) / (endTime * endTime);
        a = -(c + b * endTime) / (endTime * endTime);
        stopTime = 0;
        phase = 3;
    }

    void StopCart() {
        phase = 5;
        GetEntityByGUID(1512560813).GetSoundSource().Stop();
    }
}
