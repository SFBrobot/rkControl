typedef struct {
  float deriv,
    err,
    errThresh,
    errXing,
    input,
    integ,
    integLim,
    integXing,
    kI,
    kD,
    kDXDown,
    kDXUp,
    out,
    setpoint,
    thresh,
    xSignLim;

  bool hasXed,
    doRun,
    doUpdate,
    isOnTgt;
} Tbh;

bool isTbhInThresh(Tbh *tbh, float thresh) { return fabs(tbh->err) <= thresh; }

bool isTbhDerivInThresh(Tbh *tbh, float thresh) { return fabs(tbh->deriv) <= thresh; }

void updateTbhErr(Tbh *tbh) {
  tbh->err = tbh->setpoint - tbh->input;

  tbh->isOnTgt = isTbhInThresh(tbh, tbh->thresh);

  if (tbh->isOnTgt) tbh->errThresh = 0;
  else tbh->errThresh = tbh->err - tbh->thresh * sgn(tbh->err);
}

void setTbh(Tbh *tbh, float setpoint) {
  tbh->hasXed = tbh->hasXed && tbh->setpoint != 0;

  if (tbh->hasXed) tbh->integXing *= setpoint / tbh->setpoint; //Take a linear guess
  else tbh->integXing = 0;

  tbh->setpoint = setpoint;

  updateTbhErr(tbh);

  tbh->errXing = tbh->err; //Don't immediately trigger a crossing
}

void resetTbh(Tbh *tbh, float input) {
  tbh->input = input;
  tbh->deriv = tbh->integ = 0;

  setTbh(tbh, input);
}

void setTbhDoRun(Tbh *tbh, bool doRun) {
  tbh->doRun = doRun;

  if (doRun) tbh->doUpdate = true;
}

void initTbh(
  Tbh *tbh,
  float thresh,
  float kI,
  float kD,
  float kDXUp,
  float kDXDown,
  float integLim,
  float xSignLim) {
  tbh->integLim = integLim;
  tbh->kI = kI;
  tbh->kD = kD;
  tbh->kDXUp = kDXUp;
  tbh->kDXDown = kDXDown;
  tbh->out = 0;
  tbh->thresh = thresh;
  tbh->xSignLim = xSignLim;

  tbh->doUpdate = true;

  resetTbh(tbh, 0);
  setTbhDoRun(tbh, false);
}

void initTbh(
  Tbh *tbh,
  float thresh,
  float kI,
  float kD,
  float kDCross,
  float integLim,
  float xSignLim) {

  initTbh(tbh, thresh, kI, kD, kDCross, kDCross, integLim, xSignLim);
}

void initTbh(
  Tbh *tbh,
  float thresh,
  float kI,
  float kD,
  float integLim,
  float xSignLim) {

  initTbh(tbh, thresh, kI, kD, kD, integLim, xSignLim);
}

float updateTbh(Tbh *tbh, float input, float deriv, float dt) {
  tbh->input = input;
  tbh->deriv = deriv;

  updateTbhErr(tbh);

  if (!tbh->doRun) {
    tbh->doUpdate = false;
    return tbh->out = 0;
  }

  tbh->integ = fmaxf(
    -tbh->integLim,
    fminf(
      tbh->integLim,
      tbh->integ + (
        tbh->errThresh * tbh->kI
        - tbh->deriv * (
          (tbh->err > 0 && tbh->deriv > 0) || (tbh->err < 0 && tbh->deriv < 0)
          ? (tbh->err > 0 ? tbh->kDXUp : tbh->kDXDown)
          : tbh->kD
          )
        ) * dt
      )
    );

  if ((tbh->errXing <= 0 && tbh->err > 0) || (tbh->errXing >= 0 && tbh->err < 0)) {
    if (tbh->hasXed)
      tbh->integ = (tbh->integ + tbh->integXing) / 2.;
    else
      tbh->hasXed = true;

    tbh->integXing = tbh->integ;

    tbh->errXing = tbh->err;
  }

  tbh->out = tbh->integ;

  if (tbh->xSignLim >= 0) {
    if (tbh->input > 0) tbh->out = fmaxf(-tbh->xSignLim, tbh->out);
    else if (tbh->input < 0) tbh->out = fminf(tbh->xSignLim, tbh->out);
  }

  return tbh->out;
}
