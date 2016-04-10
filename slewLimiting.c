typedef struct {
  int pwrTarg,
    mtrPwr,
    pwrLast,
    mtrNum;
  tMotor name[3];
  tSensors encName;
} System;

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