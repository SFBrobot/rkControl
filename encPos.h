/*
"Documentation" for this
	To use it, you need to put initPos in the pre-auton, and updatePos in the autonomous and driver control while loops.
	
void initPos(Pos *pos, float convertToDegs, tSensors lEnc, tSensors rEnc)
	takes 4 arguments
		pointer to Pos struct
		arbitrary constant used to convert the difference between the encoders to degrees of the robot's rotation
		name of the left encoder
		name of the right encoder

void updateEnc(Enc *enc)
	takes 1 argument
		pointer to an Enc struct
	this function shouldn't be called on any Enc structs related to drive encoders
		it will break the deltaVal used for x and y tracking
void updatePos(Pos *pos)
	takes 1 argument
		pointer to a Pos struct
	contains its own updateEnc() function calls (why you don't need them)
*/

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

void initPos(Pos *pos, float convertToDegs, tSensors lEnc, tSensors rEnc) {
	pos->lEnc.name = lEnc;
	pos->rEnc.name = rEnc;
	pos->convertToDegs = convertToDegs;
}

void updateEnc(Enc *enc) {
	enc->valLast = enc->val;
	enc->val = SensorValue[enc->name];
	enc->deltaVal = enc->val - enc->valLast;
}

void updatePos(Pos *pos) {
	updateEnc(&pos->lEnc);
	updateEnc(&pos->rEnc);
	pos->theta = 1. ((pos->lEnc.deltaVal - pos->rEnc.deltaVal) * pos->convertToDegs) % 360;
	pos->x += .5 * (pos->lEnc.deltaVal + pos->rEnc.deltaVal) * cosDegrees(pos->theta);
	pos->y += .5 * (pos->lEnc.deltaVal + pos->rEnc.deltaVal) * sinDegrees(pos->theta);
}
