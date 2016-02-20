typedef struct {
  float *arr, out;
  int len, curr;
} RollingAverageFilter;

typedef RollingAverageFilter RAFlt;

void resetRAFlt(RAFlt *flt, float value) {
  for (int i = 0; i < flt->len; i++)
    flt->arr[i] = value;

  flt->out = value;
  flt->curr = 0;
}

void initRAFlt(RAFlt *flt, float* arr, int len) {
  flt->arr = arr;
  flt->len = len;

  resetRAFlt(flt, 0);
}

float updateRAFlt(RAFlt *flt, float value) {
  flt->arr[flt->curr] = value;
  flt->curr = (flt->curr + 1) % flt->len;

  flt->out = 0;
  for (int i = 0; i < flt->len; i++)
    flt->out += flt->arr[i];

  return flt->out /= flt->len;
}
