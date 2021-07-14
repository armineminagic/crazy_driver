int GamePlay(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, int &OPTION) {
    HDC hdc;
	static int TheEnd = 0;

	static HWND BTN[10];

	static HDC hdcBackBuffer; // hdcBuffer
	static HBITMAP hBitmap;
	PAINTSTRUCT ps;

	static HBRUSH color[50] = {};
    
	// player size
	static int RectWidth = 45;
    static int RectHeight = 45;

	static int GateInfo[2] = {};

	GateInfo[0] = 15;
	GateInfo[1] = GetRandomNumber(45,65);

	static int Level = 1;
	static int OldLevel = 1;

	static int OldDeaths = 0;
	static int Deaths = 0;


	static RECT r1 = {}, client = {}, prevState = {};

    static RECT DefPos ={};
	static RECT Container = {};
    static RECT Gate = {};
	static RECT obstacles[100] = {};
	static double obstaclesInfo[100][100] = {}; // obstacles info



	int ok = 0;
	static int move = 10; // step of the player
	static int offset[4] = {};

	offset[0] = 30; // left
	offset[1] = 35; // right
	offset[2] = 80; // top
	offset[3] = 35; // bottom

	static int width, height;


	switch (message)
	{
	case WM_NCCREATE : {
		GetSavedProgress(OldLevel, OldDeaths);
		break;
	}
	case WM_CREATE:
		{

		// we make sure that we show the corrects deaths number

			Deaths = 0;
			Level = 0;


		if(!DEFAULT_DIR_PATH[0])
			GetCurrentDirectory(MAX_PATH, DEFAULT_DIR_PATH);

		GetClientRect (hwnd, &client);

		// let's add a few buttons

		BTN[0] = CreateWindow(TEXT("button"),  TEXT(BTN_EXIT), WS_VISIBLE | WS_CHILD,
					client.right - offset[1] - 80, client.bottom - offset[3],
					80, 25, hwnd, (HMENU)END_BTN, NULL, NULL);

		BTN[2] = CreateWindow(TEXT("button"),  TEXT(BTN_RESTART), WS_VISIBLE | WS_CHILD,
					client.right - offset[1] - 190, client.bottom - offset[3],
					100, 25, hwnd, (HMENU)RESTART_BTN, NULL, NULL);

		// colors
		color[0] = CreateSolidBrush (RGB (240, 240, 240)); // grey-ish container

		color[1] = CreateSolidBrush (RGB (140, 0, 140)); // player purple

		color[2] = CreateSolidBrush(RGB(0,222,52)); // green

		color[3] = CreateSolidBrush(RGB(0,0,255)); // blue

		color[4] = CreateSolidBrush(RGB(255,0,0)); // red

		color[5] = CreateSolidBrush(RGB(0,0,0)); // black



		width = client.right;
		height = client.bottom;

		// Create the back buffer
		hdcBackBuffer = CreateCompatibleDC(NULL);

		// Get the device context
		hdc = GetDC(hwnd);

		// Create a bitmap
		hBitmap = CreateCompatibleBitmap(hdc, width, height);

		// Select the bitmap
		SelectObject(hdcBackBuffer, hBitmap);

		// position of the container
		Container.left = client.left + offset[0];
		Container.right = client.right - offset[1];
		Container.top = client.top + offset[2];
		Container.bottom = client.bottom - offset[3];

        // initial position of the player
		DefPos.right = client.left + offset[0] + RectWidth;
		DefPos.left = client.left + offset[0];
		DefPos.top = (client.top + client.bottom) /2;
		DefPos.bottom = (client.top + client.bottom)/2 + RectHeight;

		// add the gate
		generateGate(offset[2], Gate, Container, GateInfo);


		// we keep the original pos in DefPos
        r1 = DefPos;


		// generate the obstacles
		generate_obstacles(obstacles, obstaclesInfo, Level, offset, DefPos);

		// add the timer
		SetTimer(hwnd,1,UPDATE_RATE, NULL);



		ReleaseDC(hwnd, hdc);
		
		return 0;
	}

	case WM_PAINT:
		{
		//SetWindowText(BTN[1], TEXT(BTN_CREATE_LEVEL));
		hdc = BeginPaint( hwnd, &ps );

		BitBlt(hdcBackBuffer,0,0,width,height, NULL, NULL, NULL, WHITENESS);
		TheEnd = 0;
		if(!TheEnd) {
			//cout << " THE END " << endl;
			// game info

			DrawText(hdcBackBuffer, GAME_TITLE, -1, &client, DT_CENTER);
			DrawText(hdcBackBuffer, OBIECTIV, -1, &client, DT_CENTER);

			// level txt
			char tmpText[100];
			sprintf_s(tmpText, LEVEL_TXT, Level);

			DrawText(hdcBackBuffer, TEXT(tmpText), -1, &client, DT_LEFT);

			sprintf_s(tmpText, DEATHS, Deaths);
			DrawText(hdcBackBuffer, TEXT(tmpText), -1, &client, DT_RIGHT);

			// the main rectangel(aka the Container)

			FillRect(hdcBackBuffer, &Container, color[0]);
			FrameRect(hdcBackBuffer, &Container, color[5]);

			// add the player
			// DrawFrog();
			FillRect (hdcBackBuffer, &r1, color[1]);

			// add the gate
			FillRect (hdcBackBuffer, &Gate, color[2]);

			for(int i = 0; i < sizeof(obstacles)/sizeof(obstacles[0]); i++){
				if(obstaclesInfo[i][CO_TYPE] == 1) // Rectangle
					FillRect(hdcBackBuffer, &obstacles[i], color[3]);
				else if(obstaclesInfo[i][CO_TYPE] == 2) { // Ellipse
					SelectObject(hdcBackBuffer, color[4]);
					My_Ellipse(hdcBackBuffer, obstacles[i]);
				} else if(obstaclesInfo[i][CO_TYPE] == 3) { // Wall
					if(obstaclesInfo[i][CO_MOVE] == 1) {  // we interpret it as type
						FillRect(hdcBackBuffer, &obstacles[i], color[5]);
					} else if(obstaclesInfo[i][CO_MOVE] == 2) {
						SelectObject(hdcBackBuffer, color[5]);
						My_Ellipse(hdcBackBuffer, obstacles[i]);
					}
				}

			}

			// Display the back buffer
			BitBlt(hdc, 0, 0, width, height, hdcBackBuffer, 0, 0, SRCCOPY);

			EndPaint(hwnd,&ps);

			ReleaseDC(hwnd, hdc);
			ReleaseDC(hwnd, hdcBackBuffer);
			// check if it collides with an obstacle
			for(int i = 0; i < sizeof(obstacles)/sizeof(obstacles[0]); i++) {
				if(obstaclesInfo[i][CO_TYPE] != 3 && game_over(hwnd, r1, obstacles[i], DefPos)){
					// generate the obstacles
					generate_obstacles(obstacles, obstaclesInfo, Level, offset, DefPos);
					Deaths++;
					break;
				}
			}

		}

		else {



		DrawText(hdcBackBuffer, TEXT(FINISH_GAME), -1, &client, DT_CENTER);

		// Display the back buffer
		BitBlt(hdc, 0, 0, width, height, hdcBackBuffer, 0, 0, SRCCOPY);

		EndPaint(hwnd,&ps);
		ReleaseDC(hwnd, hdc);
		ReleaseDC(hwnd, hdcBackBuffer);


		}


		return 0;


		}
	case WM_KEYDOWN: {

		// the moves of the player
		prevState = r1;



		OBJMove(r1, Container, move, ok);

		//if(wParam == VK_ESCAPE) {
		//	 	cout << wParam << "  " << VK_ESCAPE << endl;
		//		r1 = DefPos;
		//		ok = 5;
		//		Deaths++;

		//}

		// we must but check the walls
		for(int i = 0; i < sizeof(obstacles)/sizeof(obstacles[0]); i++) {
			if(obstaclesInfo[i][CO_TYPE] == 3) {
				checkWall(r1, obstacles[i], ok-1, prevState);
			}
		}

		if(ok) {
			InvalidateRect(hwnd,NULL,false);
			UpdateWindow(hwnd);
		}

		// check the collison with the gate
		if(r1.right > Gate.left && do_rectangles_intersect(Gate,r1)) {
			KillTimer(hwnd,1); // stop the action

			MessageBox(hwnd, TEXT(WIN_TXT), TEXT(WIN_TXT_TITLE), MB_OK);

			char tmp[100];
			sprintf_s(tmp, "./CO_LEVELS/level%d.txt", Level+1);

			if(fileExists(tmp)) {
				// add the gate
				generateGate(offset[2], Gate, Container, GateInfo);


				SetTimer(hwnd,1,UPDATE_RATE, NULL); 		// start the action

				r1 = DefPos;
				Level++;

				// generate the obstacles
				generate_obstacles(obstacles, obstaclesInfo, Level, offset, DefPos);
			} else {
				r1.left = 0;
				r1.bottom = 0;
				TheEnd = 1;
				EnableWindow(BTN[3], FALSE);
			}

			InvalidateRect(hwnd,NULL,false);
			UpdateWindow(hwnd);
		}


	return 0;

	}
	case WM_TIMER :
	{

		for(int i = 0; i < sizeof(obstacles)/sizeof(obstacles[0]); i++) {
			if(obstaclesInfo[i][CO_TYPE] == 3) // if it's a wall
				continue; // because we use there the CO_MOVE

			if(obstaclesInfo[i][CO_MOVE] == 1)
				moveUpAndDown(obstacles[i],Container,(long)obstaclesInfo[i][CO_STEP],i,obstacles,obstaclesInfo);
			else if(obstaclesInfo[i][CO_MOVE] == 2)
				moveInCircle(obstacles[i],Container,obstaclesInfo[i][CO_STEP],obstaclesInfo[i][CO_RADIUS],i,obstacles, obstaclesInfo);
			else if(obstaclesInfo[i][CO_MOVE] == 3)
				moveBounceOnWalls(obstacles[i],Container,(long)obstaclesInfo[i][CO_STEP],i,obstacles,obstaclesInfo);
			else if(obstaclesInfo[i][CO_MOVE] == 4)
				moveFollower(obstacles[i],r1, Container,(long)obstaclesInfo[i][CO_STEP]);
			else if(obstaclesInfo[i][CO_MOVE] == 5)
				moveLeftAndRight(obstacles[i],Container,(long)obstaclesInfo[i][CO_STEP],i,obstacles,obstaclesInfo);
		}

		InvalidateRect(hwnd,NULL,false);
		UpdateWindow(hwnd);
	}

	case WM_COMMAND: { // here we can handle the buttons

		switch(wParam) {
			case END_BTN : {
				if(MessageBox(hwnd, TEXT(ARE_YOU_SURE), TEXT(CONFIRM), MB_YESNO) == IDYES){
					CleanFiles();
					PostQuitMessage (0);  // we exit
				}
				break;
			}
			case RESTART_BTN : {
				KillTimer(hwnd,1); // stop the action
				if(MessageBox(hwnd, TEXT(ARE_YOU_SURE), TEXT(CONFIRM), MB_OKCANCEL) == IDOK) {
					Deaths = 0;
					r1 = DefPos;


						CleanFiles(1); // we clean the progress file
						Level = 1;
						TheEnd = 0;

					EnableWindow(BTN[3], TRUE);
					generate_obstacles(obstacles, obstaclesInfo, Level, offset, DefPos);
				}
				SetTimer(hwnd,1,UPDATE_RATE, NULL); 		// start the action
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

		CleanFiles();
		PostQuitMessage (0);
		return 0;
	}
	return 0;
}
