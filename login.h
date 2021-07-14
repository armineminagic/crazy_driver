void AddControls(HWND &hwnd, HWND BTN[]);

int Login(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, int &OPTION) {
     switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            static HWND BTN[10] = {};
            AddControls(hwnd, BTN);
            break;
        case WM_COMMAND: { // here we can handle the buttons

		switch(wParam) {
			case LOGIN_BTN : {
				if(MessageBox(hwnd, TEXT(ARE_YOU_SURE), TEXT(CONFIRM), MB_YESNO) == IDYES){
					CleanFiles();
					PostQuitMessage (0);  // we exit
				}
				break;
			}
			case SCOREBOARD_BTN : {
                OPTION = 1;
                DestroyButtons(BTN);
                SendMessage(hwnd, WM_CREATE, wParam, lParam);
                SetFocus(hwnd);
                InvalidateRect(hwnd, NULL, false);
                UpdateWindow(hwnd);
				break;
			}
			// case ID_BTN_CREATE : {

			// 	OPTION = 1;

			// 	if(!TestLevel) {
			// 		OldDeaths = Deaths;
			// 		OldLevel = Level;
			// 	}
			// 	// clean the buttons
			// 	DestroyButtons(BTN);

			// 	KillTimer(hwnd,1); // stop the action
			// 	SendMessage(hwnd, WM_CREATE, wParam, lParam); // set it to create the new content

			// 	SetFocus(hwnd);

			// 	InvalidateRect(hwnd, NULL, false);
			// 	UpdateWindow(hwnd);

			// 	break;
			// }
			// case ID_BTN_SAVE_PROGRESS : {

			// 	SaveProgress(hwnd, Level, Deaths);

			// 	SetFocus(hwnd);

			// 	InvalidateRect(hwnd, NULL, false);
			// 	UpdateWindow(hwnd);
			// 	break;
			// }
		}

		return 0;
    }
        case WM_DESTROY:
            PostQuitMessage (0); 
            break;
    }
    return 0;
}

void AddControls(HWND &hwnd, HWND BTN[])
{
    BTN[2] = CreateWindow("Static","Player:",WS_VISIBLE|WS_CHILD|WS_BORDER,250,50,100,20,hwnd,NULL,NULL,NULL);

    BTN[3] = CreateWindow("Edit"," ",WS_VISIBLE|WS_CHILD|WS_BORDER|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,310,50,200,20,hwnd,NULL,NULL,NULL);

    BTN[4] =CreateWindow("Static","Password:",WS_VISIBLE|WS_CHILD|WS_BORDER,250,100,100,20,hwnd,NULL,NULL,NULL);

    BTN[5] =CreateWindow("Edit"," ",WS_VISIBLE|WS_CHILD|WS_BORDER|ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,350,100,160,20,hwnd,NULL,NULL,NULL);

    BTN[0] = CreateWindow("Button","Play",WS_VISIBLE|WS_CHILD,150,304,100,50,hwnd,(HMENU)LOGIN_BTN,NULL,NULL);

    BTN[1] = CreateWindow("Button","Scoreboard",WS_VISIBLE|WS_CHILD,550,304,100,50,hwnd,(HMENU)SCOREBOARD_BTN,NULL,NULL);
}