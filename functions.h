// generates a random numer between the limits
int GetRandomNumber(int nLow, int nHigh)
{
	 srand( (unsigned)time( NULL ) );
    return (rand() % (nHigh - nLow + 1)) + nLow;
}

// returns the double value from string
double strtodbl(string String) { //can be modified to make better, but this should work for simple stuff
	return ::atof(String.c_str());
}

// makes a custom ellipse
void createCustomEllipse(HDC hdc, RECT obstacle) {
	Ellipse(hdc, obstacle.left, obstacle.top, obstacle.right, obstacle.bottom);
}

// make the gate where
void makeGate(int offset, RECT &Gate, RECT Container, int GateInfo[]) {

		Gate.top = GetRandomNumber(offset,Container.bottom);
		Gate.left = Container.right - GateInfo[0];
		Gate.right = Container.right;
		if(Gate.top + GateInfo[1] > Container.bottom) {
			Gate.top -= Gate.top + GateInfo[1] - Container.bottom;
			Gate.bottom = Container.bottom;
		} else {
			Gate.bottom = Gate.top + GateInfo[1];
		}

}