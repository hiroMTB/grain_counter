void ofApp::keyPressed(int key){
    
    // machine manual control
    glm::vec3 cp = machine.currentPos;
    glm::vec3 tp = machine.targetPos;
    int xySpeed = 1000;
    int zSpeed  = 150;
    
    switch(key){
        case 'H':
        {
            HomeCommand m(machine, 0);
            m.call();
            break;
        }
        case OF_KEY_LEFT:
        {
            MoveCommand m(machine, MoveCommand::AXIS_X, 0, cp, tp.x-1, xySpeed);
            m.call();
            break;
        }
            
        case OF_KEY_RIGHT:
        {
            MoveCommand m(machine, MoveCommand::AXIS_X, 0, cp, tp.x+1, xySpeed);
            m.call();
            break;
        }
            
        case OF_KEY_UP:
        {
            MoveCommand m(machine, MoveCommand::AXIS_Y, 0, cp, tp.y+1, xySpeed);
            m.call();
            break;
        }
            
        case OF_KEY_DOWN:
        {
            MoveCommand m(machine, MoveCommand::AXIS_Y, 0, cp, tp.y-1, xySpeed);
            m.call();
            break;
        }
            
        case '[':
        {
            MoveCommand m(machine, MoveCommand::AXIS_Z, 0, cp, tp.z+1, zSpeed);
            m.call();
            break;
        }
        case ']':
        {
            MoveCommand m(machine, MoveCommand::AXIS_Z, 0, cp, tp.z-1, zSpeed);
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
    
    // video
    switch(key){
        case 'S':
            bSaveRequest = true;
            break;
        case 'F':
            ofToggleFullscreen();
            break;
        case ' ':
            bRunSequence = !bRunSequence;
            break;
    }
}
