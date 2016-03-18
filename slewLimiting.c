#define RX vexRT[Ch1]
#define RY vexRT[Ch2]
#define LY vexRT[Ch3]

#define STICK_THRESH 7

typedef struct motorSystem {
  int pwrTarg,
    mtrPwr,
    pwrLast,
    mtrNum;
  tMotor name[4];
  tSensors encName;
} System;

System lDrive,
  rDrive,
  flywheel,
  intake;

int slewLim(int mtrPwr, int pwrLast, int slewLim) {
  if(abs(mtrPwr - pwrLast) > slewLim)
    mtrPwr = pwrLast + slewLim * sgn(mtrPwr - pwrLast);
  return mtrPwr;
}
  
void setSysPwr(System *sys, int pwr, int slewLim = 0) {
  sys->pwrLast = sys->mtrPwr;
  sys->mtrTarg = pwr;
  if(slewLim != 0)
    sys->mtrPwr = slewLim(sys->mtrTarg, sys->pwrLast, slewLim);
  for(int i = 0; i < sys->mtrNum; i++) {
    motor[sys->name[i]] = sys->mtrPwr;
  }
}
  
task main() {
  int rx,
    ry,
    ly;

  lDrive.mtrNum = 3;
  rDrive.mtrNum = 3;
  flywheel.mtrNum = 2;
  intake.mtrNum = 2;
  
  lDrive.name[0] = flDrive;
  lDrive.name[1] = mlDrive;
  lDrive.name[2] = blDrive;
  lDrive.encName = lDriveEnc;
  
  rDrive.name[0] = frDrive;
  rDrive.name[1] = mrDrive;
  rDrive.name[2] = brDrive;
  rDrive.encName = rDriveEnc;
  
  flywheel.name[0] = lFly;
  flywheel.name[1] = rFly;
  flywheel.encName = flyEnc;
  
  intake.name[0] = roller;
  intake.name[1] = lift;
  intake.encName = liftEnc;
  
  while(true) {
    if(abs(RX) > STICK_THRESH)
      rx = RX;
    else
      rx = 0;
    if(abs(RY) > STICK_THRESH)
      ry = RY;
    else
      ry = 0;
    if(abs(LY) > STICK_THRESH)
      ly = LY;
    else
      ly = 0;
    
    setSysPwr(&lDrive, ly + rx, 63);
    setSysPwr(&rDrive, ly - rx, 63);
  } 
}
