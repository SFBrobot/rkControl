typedef struct {
	long val,
		valLast;
	float deltaVal;
	tSensors name;
} Encoder;
typedef Encoder Enc; //Those 4 letters are a real pain

typedef struct {
	float x,
		y,
		theta,
		convertToDegs;
	Enc lEnc,
		rEnc;
} Position;
typedef Position Pos;

void initPos(Pos *pos, float conversion, tSensors lEnc, tSensors rEnc) {
	pos->lEnc.name = lEnc;
	pos->rEnc.name = rEnc;
	pos->convertToDegs = conversion;
}

void updateEnc(Enc *enc) {
	enc->valLast = enc->val;
	enc->val = SensorValue[enc->name];
	enc->deltaVal = enc->val - enc->valLast;
}

void updatePos(Pos *pos) {
	updateEnc(pos->lEnc);
	updateEnc(pos->rEnc);
	pos->theta = ((pos->lEnc.deltaVal - pos->rEnc.deltaVal) * pos->convertToDegs) % 360;
	pos->x += .5 * (pos->lEnc.deltaVal + pos->rEnc.deltaVal) * cosDegrees(pos->theta);
	pos->y += .5 * (pos->lEnc.deltaVal + pos->rEnc.deltaVal) * sinDegrees(pos->theta);
}
