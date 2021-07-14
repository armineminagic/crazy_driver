// functions related to buttons

using namespace std;
#include <iostream>

// disable all buttons in the maker
void DisableAll(HWND hwnd, HWND BTN[]) {
    cout << "disableall se koristi " << endl;
	EnableWindow(BTN[0], FALSE);
	EnableWindow(BTN[1], FALSE);
	EnableWindow(BTN[2], FALSE);
	EnableWindow(BTN[3], FALSE);

	SendMessage(BTN[0], CB_SETCURSEL, -1, 0);
	SendMessage(BTN[1], WM_SETTEXT, 0, (LPARAM)"");
	SendMessage(BTN[2], WM_SETTEXT, 0, (LPARAM)"");
	CheckDlgButton(hwnd, ID_BTN_WALL, BST_UNCHECKED);
}

// destroy the buttons on the current view
void DestroyButtons(HWND BTN[]){
    cout << "destroybuttons se koristi " << endl;
	for( int i = 0; i <= 10; i++ ) {
		DestroyWindow(BTN[i]);
	}

}
