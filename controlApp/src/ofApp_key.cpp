void ofApp::keyPressed(int key){
    
    if(key == ' '){
        bRunSequence = !bRunSequence;
    }
    
    // machine manual control
    if(machine.serial.isInitialized()){
        
        glm::vec3 tp = machine.targetPos;
        int xySpeed = 300;
        int zSpeed  = 150;
        
        switch(key){
            case 'F':
                ofToggleFullscreen();
                break;
                
            case 'H':
            {
                HomeCommand m(machine, 0);
                m.call();
                break;
            }
            case OF_KEY_LEFT:
            {
                MoveCommand m(machine, MoveCommand::AXIS_X, 0, tp, tp.x-1, xySpeed);
                m.call();
                break;
            }
                
            case OF_KEY_RIGHT:
            {
                MoveCommand m(machine, MoveCommand::AXIS_X, 0, tp, tp.x+1, xySpeed);
                m.call();
                break;
            }
                
            case OF_KEY_UP:
            {
                MoveCommand m(machine, MoveCommand::AXIS_Y, 0, tp, tp.y-1, xySpeed);
                m.call();
                break;
            }
                
            case OF_KEY_DOWN:
            {
                MoveCommand m(machine, MoveCommand::AXIS_Y, 0, tp, tp.y+1, xySpeed);
                m.call();
                break;
            }
                
            case '[':
            {
                MoveCommand m(machine, MoveCommand::AXIS_Z, 0, tp, tp.z-1, zSpeed);
                m.call();
                break;
            }
            case ']':
            {
                MoveCommand m(machine, MoveCommand::AXIS_Z, 0, tp, tp.z+1, zSpeed);
                m.call();
                break;
            }
                
            case OF_KEY_RETURN:
            {
                SuckCommand m(machine, 0, tp, !machine.bSuck);
                m.call();
                break;
            }
                
            case 'n':
            {
                shared_ptr<Command> command = cmds[currentCmd];
                command->call();
                currentFrame = command->frame;
                currentCmd++;
                currentCmd = currentCmd % cmds.size();
                cout << command->cmd << endl;
                break;
            }
        }
    }
    
    // video
    switch(key){
        case 'S':
            bSaveRequest = true;
            break;
    }
}
