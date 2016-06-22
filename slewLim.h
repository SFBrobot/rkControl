#ifndef SLEW_LIM
#define SLEW_LIM

int mtrSlew(tMotor mtr, int target, int maxSlew) {
	if(abs(target - motor[mtr]) > maxSlew) {
		if(target > motor[mtr])
			return motor[mtr] += maxSlew;
		else
			return motor[mtr] -= maxSlew;
	}
	else
		return motor[mtr] = target;
}

#endif
