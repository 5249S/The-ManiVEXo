/*--------------------------------------------*/
/*                    5249S                   */
/*              Robotic ManiVEXo              */
/*                Driver/Auton                */
/*                Version 0.3.0               */
/*--------------------------------------------*/

void auton(int autonMode){
    ctrPrimary.Screen.clearScreen();
    ctrPrimary.Screen.setCursor(1,0);
    ctrPrimary.Screen.print("Autonomous");
    if (autonMode == 1){//Run Auton 1
        //Declare variable here
        robotMain.Screen.clearScreen();
        robotMain.Screen.setCursor(1,0);
        int clock = 0;//Reset clock, motors, and process
        int process = 0; //variable to control where in the auton you are
        mtrDriveLeft.resetRotation();
        mtrDriveRight.resetRotation();
        mtrClaw.resetRotation();
        mtrLiftLeft.resetRotation();
        mtrLiftRight.resetRotation();
        driveYawPID.reset();
        driveSpeedPID.reset();
        mtrLauncherFire.resetRotation();
        while (confirmAuton() && process < 7){//Set process number to last process
            if(process == 0){
                driveForward(-1, 100);
                process ++;
            }
            if (process == 1){
                if (!mtrDriveLeft.isSpinning() && !mtrDriveRight.isSpinning()){
                    clock = 0;
                    process ++;
                }
            }
            if (process == 2){
                if (clock >= 100){
                    driveTurn(90, 100);
                    process ++;
                }
            }
            if (process == 3){
                if (!mtrDriveLeft.isSpinning() && !mtrDriveRight.isSpinning()){
                    clock = 0;
                    process ++;
                }
            }
            if (process == 4){
                if (clock >= 100){
                    driveForward(-1.5, 100);
                    process ++;
                }
            }
            if (process == 5){
                if (!mtrDriveLeft.isSpinning() && !mtrDriveRight.isSpinning()){
                    robot.setLiftLevel(5);
                    process ++;
                }
            }
            robotMain.Screen.clearScreen();
            robotMain.Screen.setCursor(1,0);
            robotMain.Screen.print("Left: %f", mtrDriveLeft.rotation(vex::rotationUnits::deg));
            robotMain.Screen.newLine();
            robotMain.Screen.print("Right: %f", mtrDriveRight.rotation(vex::rotationUnits::deg));
            wait(20);
            clock += 20;
        }
    }
}

void driver(){
    //Declare variables here
    ctrPrimary.Screen.clearScreen();
    ctrPrimary.Screen.setCursor(0,0);
    ctrPrimary.Screen.print("Party Time");//Party Time
    bool liftMode = false;
    bool waitForReleaseA = false;
    bool waitForReleaseUp = false;
    bool waitForReleaseDown = false;
    bool waitForReleaseRight = false;
    bool waitForReleaseLeft = false;
    bool waitForReleaseY = false;
    int liftLevel = 0;
    bool holdLift = false;
    bool clawFlipped = false;
    bool reverseClawMotor = false;
    ctrPrimary.Screen.setCursor(3,0);
    ctrPrimary.Screen.clearLine(3);
    ctrPrimary.Screen.print("Launch");
    while (confirmDriver()){
        //Run driver implementation here
        if (ctrPrimary.ButtonA.pressing() && !waitForReleaseA){//Turns on auto target
            waitForReleaseA = true;
            liftMode = !liftMode;
            ctrPrimary.Screen.setCursor(3,0);
            ctrPrimary.Screen.clearLine(3);
            if(liftMode){
                ctrPrimary.Screen.print("Lift: %d", liftLevel);
            } else {
                ctrPrimary.Screen.print("Launch");
            }
        }
        if (!ctrPrimary.ButtonA.pressing() && waitForReleaseA){//Waits for release to change the state of auto target
            waitForReleaseA = false;
        }
        if (!liftMode){//Run limited chassis
            targetSystem.scanForFlags();//Run target system
            double angle = targetSystem.targetSpecificFlag();
            if (!ctrPrimary.ButtonR1.pressing() && !ctrPrimary.ButtonR2.pressing()){//If no manual controls are being pressed, Angle the ball launcher
                if (angle != -1 && !ctrPrimary.ButtonX.pressing()){
                    setLauncherToAngle(angle);
                } else {
                    mtrLauncherAngle.stop(vex::brakeType::hold);
                }
            } else {
                robot.launchAngle(ctrPrimary.ButtonR1.pressing(), ctrPrimary.ButtonR2.pressing());//Angle launcher manually
            }
            robot.lift(ctrPrimary.Axis2.position(vex::percentUnits::pct));
        } else {
            if (holdLift){
                if (abs(ctrPrimary.Axis2.position(vex::percentUnits::pct)) > 5){
                    holdLift = false;
                }
            }
            if (!holdLift){
                robot.lift(ctrPrimary.Axis2.position(vex::percentUnits::pct));
            }
            if (ctrPrimary.ButtonRight.pressing() && !waitForReleaseRight){
                robot.setLiftLevel(liftLevel);
                holdLift = true;
                waitForReleaseRight = true;
            }
            if (!ctrPrimary.ButtonRight.pressing() && waitForReleaseRight){
                waitForReleaseRight = false;
            }
            if (ctrPrimary.ButtonUp.pressing() && !waitForReleaseUp){
                if(liftLevel == 5){
                    liftLevel = 0;
                } else {
                    liftLevel ++;
                }
                ctrPrimary.Screen.setCursor(3,0);
                ctrPrimary.Screen.clearLine(3);
                ctrPrimary.Screen.print("Lift: %d", liftLevel);
                waitForReleaseUp = true;
            }
            if (!ctrPrimary.ButtonUp.pressing() && waitForReleaseUp){
                waitForReleaseUp = false;
            }
            if (ctrPrimary.ButtonDown.pressing() && !waitForReleaseDown){
                if(liftLevel == 0){
                    liftLevel = 5;
                } else {
                    liftLevel --;
                }
                ctrPrimary.Screen.setCursor(3,0);
                ctrPrimary.Screen.clearLine(3);
                ctrPrimary.Screen.print("Lift: %d", liftLevel);
                waitForReleaseDown = true;
            }
            if (!ctrPrimary.ButtonDown.pressing() && waitForReleaseDown){
                waitForReleaseDown = false;
            }
        }
        if (ctrPrimary.ButtonY.pressing() && !waitForReleaseY){
            reverseClawMotor = !reverseClawMotor;
            mtrClaw.setReversed(reverseClawMotor);
            mtrClaw.setRotation(0, vex::rotationUnits::deg);
            mtrLiftLeft.setRotation(0, vex::rotationUnits::deg);
            mtrLiftRight.setRotation(0, vex::rotationUnits::deg);
            waitForReleaseY = true;
        }
        if (!ctrPrimary.ButtonY.pressing() && waitForReleaseY){
            waitForReleaseY = false;
        }
        if (ctrPrimary.ButtonLeft.pressing() && !waitForReleaseLeft){
            clawFlipped = !clawFlipped;
            robot.flipClaw(clawFlipped);
            waitForReleaseLeft = true;
        }
        if (!ctrPrimary.ButtonLeft.pressing() && waitForReleaseLeft){
            waitForReleaseLeft = false;
        }
        int y = ctrPrimary.Axis3.position(vex::percentUnits::pct) * (liftMode?-1:1);//Run chassis with joystick
        int x = ctrPrimary.Axis4.position(vex::percentUnits::pct);
        
        robot.driveH(y, x);
        //robot.launchAngle(ctrPrimary.ButtonR1.pressing(), ctrPrimary.ButtonR2.pressing());//Angle launcher manually
        robot.liftBall(ctrPrimary.ButtonL1.pressing(), ctrPrimary.ButtonL2.pressing());//Ball Lift
        robot.launchFire(ctrPrimary.ButtonX.pressing());//Fire the launcher
        //robot.claw(ctrPrimary.ButtonUp.pressing(), ctrPrimary.ButtonDown.pressing());//Run the claw
        runDiagnostics();//Check for warnings and display
        robotMain.Screen.clearScreen();//Display the gyros value on the processor
        robotMain.Screen.setCursor(1,0);
        robotMain.Screen.print("Gyro: %d", gyroNav.value(vex::analogUnits::range12bit));
        robotMain.Screen.newLine();
        robotMain.Screen.print("Left: %f", getAccelTiltAngle());
        wait(20);//run at 50 Hz
    }
}
