// functions related to keyboard keys
using namespace std;
#include <iostream>

void KeyMoveLeft(int hasFrame[][100], RECT r[], int i, int id, int step = 1) {
    cout << "keymoveleft se koristi " << endl;
	if(hasFrame[id][i] == 1){
		r[i].left -= step;
		r[i].right -= step;
	}

}

void KeyMoveRight(int hasFrame[][100], RECT r[], int i, int id, int step = 1) {
    cout << "keymoveright se koristi " << endl;
	if(hasFrame[id][i] == 1){
		r[i].left += step;
		r[i].right += step;
	}

}

void KeyMoveUp(int hasFrame[][100], RECT r[], int i, int id, int step = 1) {
    cout << "keymoveup se koristi " << endl;
	if(hasFrame[id][i] == 1){
		r[i].top -= step;
		r[i].bottom -= step;
	}

}

void KeyMoveDown(int hasFrame[][100], RECT r[], int i, int id, int step = 1) {
    cout << "keymovedown se koristi " << endl;
	if(hasFrame[id][i] == 1){
		r[i].top += step;
		r[i].bottom += step;
	}

}
