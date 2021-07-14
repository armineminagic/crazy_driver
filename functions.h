/*
*
* Cross Over C++ game
* Author: Mihai Ionut Vilcu
* March-April 2013
*
*/
using namespace std;
#include <iostream>

// general use functions


// generates a random numer between the limits
int GetRandomNumber(int nLow, int nHigh)
{
	 srand( (unsigned)time( NULL ) );
    return (rand() % (nHigh - nLow + 1)) + nLow;
}

// splits the string an array based on the second argument as a delimitator
vector <string> explode(const string& str, const char& ch) {
    string next;
    vector<string> result;

    // For each character in the string
    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        } else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
         result.push_back(next);
    return result;
}

// removes the extra chars from a string
string trim(const string& str, const string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

// returns the double value from a
double strtodbl(string String) { //can be modified to make better, but this should work for simple stuff
	return ::atof(String.c_str());
}


// makes a simple ellipse
void My_Ellipse(HDC hdc, RECT obstacle) {
	Ellipse(hdc, obstacle.left, obstacle.top, obstacle.right, obstacle.bottom);

}

// generates the gate where the player is supposed to go
void generateGate(int offset, RECT &Gate, RECT Container, int GateInfo[]) {

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