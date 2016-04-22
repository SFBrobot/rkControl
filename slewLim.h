typedef struct {
  float val,
    valLast,
    max;
} SlewLimiter;

typedef SlewLimiter SLim;

void initSLim(SLim *lim, float maxSlew) {
  lim->max = maxSlew;
}

float setSLim(SLim *lim, float setPoint) {
  lim->valLast = lim->val;
  lim->val = setPoint;
  if(fabs(lim->val - lim->valLast) > lim->max) {
    if(lim->val > lim->valLast)
      return lim->val = lim->valLast + lim->max;
    else
      return lim->val = lim->valLast - lim->max;
  }
}