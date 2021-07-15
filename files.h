// File management

void CleanFiles(int onlyProgress = 0) {
	// we make sure we are in the right folder
	SetCurrentDirectory(DEFAULT_DIR_PATH);

	if(onlyProgress){
		DeleteFile("info.dat");
	}
}

// save the progress of the gameplay
BOOL SaveProgress(HWND hwnd, int Level, int Deaths) {
	// we make sure we are in the right folder
	SetCurrentDirectory(DEFAULT_DIR_PATH);

	string pszText;
	int TextLength = 1;
	char tmp[100];

	sprintf_s(tmp, "%d|%d", Level, Deaths);

	pszText.append(tmp);

	char szFileName[100] = "info.dat";

        HANDLE hFile;
		hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                           FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE) {

		TextLength = pszText.size();

		if(TextLength > 0) {
			DWORD dwWritten;
			if(!WriteFile(hFile, pszText.c_str(), TextLength, &dwWritten, NULL))
				MessageBox(hwnd,TEXT(MSG_ERROR),TEXT(MSG_ERROR),MB_OK);

		}

		CloseHandle(hFile);
		MessageBox(hwnd,TEXT(MSG_PROGRESS_SAVED),TEXT(MSG_SUCCESS),MB_OK);
		return TRUE;
	} else {
		MessageBox(hwnd,TEXT(MSG_ERROR_WRITE),TEXT(MSG_ERROR),MB_OK);
		return FALSE;
	}

   return FALSE;

}

// Get saved game
void GetSavedGame( int &Level, int &Deaths) {
	SetCurrentDirectory(DEFAULT_DIR_PATH);

	ifstream hFile("info.dat");

	if(!hFile){
		cout << "info.dat does not exists" << endl;
		return;
	}

	vector <string> txt;
	string line;

	while(getline(hFile, line)){
		if(line.find_first_of("|") != string::npos) {
			txt = trim(line, '|');
			Level = (int)strtodbl(txt[0]);
			Deaths = (int)strtodbl(txt[1]);
			break;
		}

	}
}

BOOL FileExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// generate obstacles
void makeObstacles(RECT obstacles[], double obstaclesInfo[100][100], int Level, int offset[], RECT DefPos) {
	RECT a = {};
	for(int i = 0; i < 100; i++) {
		obstacles[i] = a;
		fill_n(obstaclesInfo[i], 5, 0);
	}
	
	moveInCircle(obstacles[0], obstacles[0], 1, 1, 1, obstacles, obstaclesInfo, 1);
	moveUpAndDown(obstacles[0], obstacles[0], 1, 1, obstacles, obstaclesInfo, 1);
	moveLeftAndRight(obstacles[0], obstacles[0], 1, 1, obstacles, obstaclesInfo, 1);
	moveBounceOnWalls(obstacles[0], obstacles[0], 1, 1, obstacles, obstaclesInfo, 1);


	SetCurrentDirectory(DEFAULT_DIR_PATH);

	if(!opendir("LEVELS")) {
		
		_mkdir("LEVELS");
		HANDLE hFile;

		DWORD wmWritten;

		char strData[] = "Rectangle (150,200,30,18,1,10)\r\nEllipse(150,150,50,50,2|30,5)\r\n";

		hFile = CreateFile("LEVELS/level1.txt",GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

		WriteFile(hFile,strData,(DWORD)(sizeof(strData)),&wmWritten,NULL);

		CloseHandle(hFile);

	}

	char filename[100];
	sprintf_s(filename, "LEVELS/level%d.txt", Level);
	cout << filename << endl;

	ifstream hFile(filename);

	if(!hFile)
		return;

	string line;
	int i = 0;
	string name = "";
	double left, top, right, bottom, mv, radius, step;
	double extra = 0;

	while(getline(hFile, line)){
		objName = trim(line, '(');
		getInfo(line, name, left, top, right, bottom, mv, radius, step);
		
		if(name == "Rectangle")
            obstaclesInfo[i][CO_TYPE] = 1;
        else if (name == "Ellipse")
            obstaclesInfo[i][CO_TYPE] = 2;
        else if (name == "Wall")
            obstaclesInfo[i][CO_TYPE] = 3;
        else
            continue;

        obstacles[i].left = (long)(offset[0] + left);
        obstacles[i].right = (long)(obstacles[i].left + right);
		obstacles[i].top = (long)(offset[2] + top);
		obstacles[i].bottom = (long)(obstacles[i].top + bottom);

        if (radius != 0) {
			obstaclesInfo[i][O_MOVE] = mv;
			extra = obstaclesInfo[i][O_RADIUS] = radius;
		} else {
			obstaclesInfo[i][O_MOVE] = mv;
		}

        while(rectanglesOverlap(playerPos, obstacles[i])) {
			obstacles[i].left += (long)(45 + extra);
			obstacles[i].right += (long)(45 + extra);
		}
        if(obstaclesInfo[i][O_TYPE] == 3) {
			obstaclesInfo[i][O_STEP] = 0;
			++i;
			continue;
		}

        obstaclesInfo[i][O_STEP] = step;
        name = "";
        left = top = right = bottom = mv = radius = step = 0;
		i++;
	}

}

void getInfo(const string& line, string &name, double& left, double& top, double& right, double& bottom, double& mv, double& radius, double& step)
{
    auto currentIt = line.begin();
    string tmp;
    if (currentIt != line.end())
    {
        // Get name
        name = "";
        while(*currentIt != '(' && currentIt != line.end())
        {
            name += *currentIt;
            currentIt++;
        }

        currentIt++;
        // Get left
        while(*currentIt != ',' && currentIt != line.end())
        {
            tmp += *currentIt;
            currentIt++;
        }
        left = std::stod(tmp);
        tmp = "";

        // Get top
        currentIt++;
        while(*currentIt != ',' && currentIt != line.end())
        {
            tmp += *currentIt;
            currentIt++;
        }
        top = std::stod(tmp);
        tmp = "";

        // Get right
        currentIt++;
        while(*currentIt != ',' && currentIt != line.end())
        {
            tmp += *currentIt;
            currentIt++;
        }
        right = std::stod(tmp);
        tmp = "";

        // Get bottom
        currentIt++;
        while(*currentIt != ',' && currentIt != line.end())
        {
            tmp += *currentIt;
            currentIt++;
        }
        bottom = std::stod(tmp);
        tmp = "";

        // Check if it is wall
        if (name == "Wall"){
            currentIt++;
            while(*currentIt != ')' && currentIt != line.end()){
                tmp += *currentIt;
                currentIt++;
            }
            mv = std::stod(tmp);
            step = 0;
            radius = 0;
            tmp = "";
            return;
        }

        // Check if radius exists
        currentIt++;
        if (line.find('|') != std::string::npos)
        {
            // Get move
            while(*currentIt != '|' && currentIt != line.end())
            {
                tmp += *currentIt;
                currentIt++;
            }
            mv = std::stod(tmp);
            tmp = "";

            // Get radius
            currentIt++;
            while(*currentIt != ',' && currentIt != line.end())
            {
                tmp += *currentIt;
                currentIt++;
            }
            radius = std::stod(tmp);
            tmp = "";

        }
        else
        {
            radius = 0;
            // Get move
            while(*currentIt != ',' && currentIt != line.end())
            {
                tmp += *currentIt;
                currentIt++;
            }
            mv = std::stod(tmp);
            tmp = "";
        }

        // Get step
        currentIt++;
        while(*currentIt != ')' && currentIt != line.end())
        {
            tmp += *currentIt;
            currentIt++;
        }
        step = std::stod(tmp);
        tmp = "";

    }
    return;
}
