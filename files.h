// functions related to file handling
#include <iostream>
// clean the tmp files
void CleanFiles(int onlyProgress = 0) {
    cout << "CleanFiles se koristi" << endl;
	// we make sure we are in the right folder
	SetCurrentDirectory(DEFAULT_DIR_PATH);

	if(onlyProgress)
		DeleteFile("info.dat");
	else
		DeleteFile("CO_LEVELS/tmp.txt");

}

// save the progress of the gameplay
BOOL SaveProgress(HWND hwnd, int Level, int Deaths) {
    cout << "SaveProgress se koristi" << endl;
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


	// Write contents of text output box to this file
	// Double check that file was created
	if(hFile != INVALID_HANDLE_VALUE) {

		TextLength = pszText.size();

		// Do nothing if container is empty
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

// grab the saved progress
void GetSavedProgress( int &Level, int &Deaths) {
    cout << "GEtsaveprogress se koristi" << endl;
	// we make sure we are in the right folder
	SetCurrentDirectory(DEFAULT_DIR_PATH);
	// we read the level file
	ifstream hFile("info.dat");

	if(!hFile)
		return;

	vector <string> txt;
	string line;

	// read the lines
	while(getline(hFile, line)){
		if(line.find_first_of("|") != string::npos) {
			txt = explode(line, '|');
			Level = (int)strtodbl(txt[0]);
			Deaths = (int)strtodbl(txt[1]);
			break;
		}

	}
}


// check if dir exists
bool dirExists(const std::string& dirName_in)
{
    cout << "DirExists se koristi" << endl;
  DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
  if (ftyp == INVALID_FILE_ATTRIBUTES)
    return false;  //something is wrong with your path!

  if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    return true;   // this is a directory!

  return false;    // this is not a directory!
}

// checks if a file exists
BOOL fileExists(LPCTSTR szPath)
{
    cout << "Fileexists se koristi" << endl;
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// generate obstacles

void generate_obstacles(RECT obstacle[], double obstaclesInfo[100][100], int Level, int offset[], RECT DefPos) {
    cout << "Generate_Obstacles se koristi" << endl;
	// we first clear the obstacles
	RECT a = {};
	for(int i = 0; i < 100; i++) {
		obstacle[i] = a;
		fill_n(obstaclesInfo[i], 5, 0);
	}
	// we also clear the static arrays in the move functions
	moveInCircle(obstacle[0], obstacle[0], 1, 1, 1, obstacle, obstaclesInfo, 1); // we use some dump values who are not used anyway

	moveUpAndDown(obstacle[0], obstacle[0], 1, 1, obstacle, obstaclesInfo, 1);
	moveLeftAndRight(obstacle[0], obstacle[0], 1, 1, obstacle, obstaclesInfo, 1);
	moveBounceOnWalls(obstacle[0], obstacle[0], 1, 1, obstacle, obstaclesInfo, 1);



	// we make sure we are in the right folder
	SetCurrentDirectory(DEFAULT_DIR_PATH);

	if(!dirExists("CO_LEVELS") || !fileExists("CO_LEVELS/level1.txt")) {
		// if we have no leves or no level1
		_mkdir("CO_LEVELS");
		HANDLE hFile;

		DWORD wmWritten;

		char strData[] = "Rectangle (200,100,20,40,1,10)\r\nEllipse(150,150,50,50,2|30,5)\r\n";

		hFile = CreateFile("CO_LEVELS/level1.txt",GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

		WriteFile(hFile,strData,(DWORD)(sizeof(strData)),&wmWritten,NULL);

		CloseHandle(hFile);

	}

	char filename[100];
	sprintf_s(filename, "CO_LEVELS/level%d.txt", Level);

	// we read the level file
	ifstream hFile(filename);

	if(!hFile)
		return;


	string line;
	int i = 0;
	vector <string> objName, objInfo, objInfo2;
	double extra = 0;

	// read the lines
	while(getline(hFile, line)){

		std::cout << line << endl;
		objName = explode(line, '(');

		if(trim(objName[0]) == "Rectangle")
			obstaclesInfo[i][CO_TYPE] = 1;
		else if(trim(objName[0]) == "Ellipse")
			obstaclesInfo[i][CO_TYPE] = 2;
		else if(trim(objName[0]) == "Wall")
			obstaclesInfo[i][CO_TYPE] = 3;
		else
			continue;


		objInfo = explode(objName[1], ',');
		obstacle[i].left = (long)(offset[0] + strtodbl(trim(objInfo[0])));
		cout << "OBJ INFO left: "  << objInfo[0] << " TOP " << objInfo[1]<< endl;
		obstacle[i].top = (long)(offset[2] + strtodbl(trim(objInfo[1])));
		obstacle[i].right = (long)(obstacle[i].left + strtodbl(trim(objInfo[2])));
		obstacle[i].bottom = (long)(obstacle[i].top + strtodbl(trim(objInfo[3])));



		if(objInfo[4].find_first_of("|") != string::npos) { // if it's a circle get the radius
			objInfo2 = explode(objInfo[4], '|');
			obstaclesInfo[i][CO_MOVE] = strtodbl(trim(objInfo2[0]));
			extra = obstaclesInfo[i][CO_RADIUS] = strtodbl(trim(objInfo2[1]));
		} else{
			obstaclesInfo[i][CO_MOVE] = strtodbl(trim(objInfo[4]));
		}


		// check if the rectangles ovelap when drown we don't want that
		while(do_rectangles_intersect(DefPos, obstacle[i])) {
			obstacle[i].left += (long)(45 + extra);
			obstacle[i].right += (long)(45 + extra);

		}

		// if it's a wall we have no movement therefor the step is not required
		if(obstaclesInfo[i][CO_TYPE] == 3) {
			obstaclesInfo[i][CO_STEP] = 0;
			i++;
			continue;
		}

		obstaclesInfo[i][CO_STEP] = strtodbl(trim(objInfo[5], " \t)")); // the step

		i++;

	}


}
