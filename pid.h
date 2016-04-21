typedef struct {
  float deriv,
    err,
    errThresh,
    errXing,
    input,
    integ,
    integLim,
    kP,
    kI,
    kD,
    setpoint,
    thresh,
    out;

  bool doSgnLock,
    doRun,
    doUpdate,
    isOnTgt;
} Pid;

bool isPidInThresh(Pid *pid, float thresh) { return fabs(pid->err) <= thresh; }

bool isPidIntegInThresh(Pid *pid, float thresh) { return fabs(pid->integ) <= thresh; }

bool isPidDerivInThresh(Pid *pid, float thresh) { return fabs(pid->deriv) <= thresh; }

void updatePidErr(Pid *pid) {
  pid->err = pid->setpoint - pid->input;

  pid->isOnTgt = isPidInThresh(pid, pid->thresh);

  if (pid->isOnTgt) pid->errThresh = 0;
  else pid->errThresh = pid->err - pid->thresh * sgn(pid->err);
}

void setPid(Pid *pid, float setpoint) {
  pid->setpoint = setpoint;
  pid->integ = 0;

  updatePidErr(pid);
}

void movePid(Pid *pid, float by) {
	setPid(pid, pid->setpoint + by);
}

void resetPid(Pid *pid, float input) {
  pid->input = input;
  pid->deriv = pid->integ = 0;

  setPid(pid, input);
}

void setPidDoRun(Pid *pid, bool doRun) {
  pid->doRun = doRun;

  if (doRun) pid->doUpdate = true;
}

void initPid(
  Pid *pid,
  float thresh,
  float kP,
  float kI,
  float kD,
  float integLim,
  bool doSgnLock) {
  pid->integLim = integLim;
  pid->kP = kP;
  pid->kI = kI;
  pid->kD = kD;
  pid->thresh = thresh;
  pid->out = 0;

  pid->doSgnLock = doSgnLock;
  pid->doUpdate = true;

  resetPid(pid, 0);
  setPidDoRun(pid, false);
}

float updatePid(Pid *pid, float input, float deriv, float dt) {
  pid->input = input;
  pid->deriv = deriv;

  updatePidErr(pid);

  if (!pid->doRun) {
    pid->doUpdate = false;
    return pid->out = 0;
  }

  pid->integ = fmaxf(-pid->integLim, fminf(pid->integLim, pid->integ + pid->errThresh * pid->kI * dt));

  if ((pid->errXing <= 0 && pid->err > 0) || (pid->errXing >= 0 && pid->err < 0)) {
    pid->integ = 0;
    pid->errXing = pid->err;
  }

  pid->out = pid->errThresh * pid->kP + pid->integ - pid->deriv * pid->kD;

  if (pid->doSgnLock) {
    if (pid->input > 0 && pid->out < 0) pid->out = 0;
    else if (pid->input < 0 && pid->out > 0) pid->out = 0;
  }

  return pid->out;
}
