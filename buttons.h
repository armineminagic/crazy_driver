// destroy the buttons on the current view
void DestroyButtons(HWND BTN[]){
	for( int i = 0; i <= 10; i++ ) {
		DestroyWindow(BTN[i]);
	}

}
