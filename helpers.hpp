using namespace std;

#define O_MOVE 0 // obstacle move
#define O_TYPE 1 // obstacle type
#define O_STEP 2
#define O_RADIUS 3 // radius

// Helper for random numbers
int GetRandomNumber(int nLow, int nHigh)
{	
	srand((unsigned)time(NULL));
    return (rand() % (nHigh - nLow + 1)) + nLow;
}

//Player moving functions
void PlayerMove(RECT &curPostion, RECT gameBox, int step, int &key_ctrl){
    if (GetAsyncKeyState(VK_LEFT) && !outOfGameBox(curPostion, gameBox, 0, step)){
        curPostion.left -= step;
        curPostion.right -= step;
        key_ctrl = 1;
    }

    if (GetAsyncKeyState(VK_RIGHT) && !outOfGameBox(curPostion, gameBox, 1, step)){
        curPostion.left += step;
        curPostion.right += step;
        key_ctrl = 2;
    }

    if (GetAsyncKeyState(VK_UP) && !outOfGameBox(curPostion, gameBox, 2, step)){
        curPostion.top -= step;
        curPostion.bottom -= step;
        key_ctrl = 3;
    }

    if (GetAsyncKeyState(VK_DOWN) && !outOfGameBox(curPostion, gameBox, 3, step)){
        curPostion.top += step;
        curPostion.bottom += step;
        key_ctrl = 4;
    }
}

void wallPassed(RECT &curPosition, RECT Obstacle, int dir, RECT pos){
    if (rectanglesOverlap(curPosition, Obstacle)){
        if((dir == 3 || dir == 2) && (pos.right <= Obstacle.left))
            dir = 1;
        if ((dir == 3 || dir == 2) && (pos.left >= Obstacle.right))
            dir = 0;

        if (dir == 0 && curPosition.left < Obstacle.right) {
            curPosition.right += Obstacle.right - curPosition.left;
            curPosition.left = Obstacle.right;
        }

        if (dir == 1 && curPosition.right > Obstacle.left) {
            curPosition.left -= curPosition.right - Obstacle.left;
            curPosition.right = Obstacle.left;
        }

        if (dir == 2 && curPosition.top < Obstacle.bottom) {
            curPosition.bottom += Obstacle.bottom - curPosition.top;
            curPosition.top = Obstacle.bottom;
        }

        if (dir == 3 && curPosition.bottom > Obstacle.top) {
            curPosition.top -= curPosition.bottom - Obstacle.top;
            curPosition.bottom = Obstacle.top;
        }
    }
}

// Generating Obstacles
void generateGate(int margin, RECT &Gate, int GateInfo[],  RECT GameBox){
    
    Gate.top = GetRandomNumber(margin, GameBox.bottom);
    Gate.left = GameBox.right - GateInfo[0];
    Gate.right = GameBox.right;
    if (Gate.top + GateInfo[1] > GameBox.bottom){
        Gate.top -= Gate.top + GateInfo[1] - GameBox.bottom;
        Gate.bottom = GameBox.bottom;
    } else {
        Gate.bottom = Gate.top + GateInfo[1];
    }
}

void generateObstacles(RECT Obstacles[], double ObstaclesInfo[100][100], int Level, int margins[], RECT playerPos){
    RECT tempObstacle = {};

    for (int i = 0; i < 100; ++i){
        Obstacles[i] = tempObstacle;
        fill_n(ObstaclesInfo[i], 5, 0);
    }

    moveUpAndDown(Obstacles[0], Obstacles[0], 1, 1, Obstacles, ObstaclesInfo, 1);
    moveRightAndLeft(Obstacles[0], Obstacles[0], 1, 1, Obstacles, ObstaclesInfo, 1);
    moveInCircle(Obstacles[0], Obstacles[0], 1, 1, 1, Obstacles, ObstaclesInfo, 1);
    moveBounceOnWalls(Obstacles[0], Obstacles[0], 1, 1, Obstacles, ObstaclesInfo, 1);

    SetCurrentDirectory(CURRENT_DIR_PATH);

    if(!directoryExists("LEVELS") || !fileExists("LEVELS/leve1.txt")){
        _mkdir("LEVELS");

        HANDLE hFile;

        DWORD wmWritten;

		char strData[] = "Rectangle (200,100,20,40,1,10)\r\nEllipse(150,150,50,50,2|30,5)\r\n";

        hFile = CreateFile("LEVELS/level1.txt", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        WriteFile(hFile, strData, (DWORD)(sizeof(strData)), &wmWritten, NULL);

        CloseHandle(hFile);
    }

    char filename[100];
    sprintf_s(filename, "LEVELS/level%d.txt", Level);

    ifstream hFile(filename);

    if(!hFile)
        return;
    
    string line;
    int i = 0;
    vector<string> objName, objInfo, objInfo2;
    double extra = 0;

    while(getline(hFile, line)){

        objName = splitString(line, '(');

        if(trim(objName[0]) == "Rectangle")
            ObstaclesInfo[i][O_TYPE] = 1;
        else if (trim(objName[0]) == "Ellipse")
            ObstaclesInfo[i][O_TYPE] = 2;
        else if (trim(objName[0]) == "Wall")
            ObstaclesInfo[i][O_TYPE] = 3;
        else
            continue;

        objInfo = splitString(objName[1], ',');
        Obstacles[i].left = (long)(margins[0] + strtodbl(trim(objInfo[0])));
        Obstacles[i].right = (long)(Obstacles[i].left + strtodbl(trim(objInfo[2])));
		Obstacles[i].top = (long)(margins[2] + strtodbl(trim(objInfo[1])));
		Obstacles[i].bottom = (long)(Obstacles[i].top + strtodbl(trim(objInfo[3])));

        if(objInfo[4].find_first_of("|") != string::npos) {
			objInfo2 = splitString(objInfo[4], '|');
			ObstaclesInfo[i][O_MOVE] = strtodbl(trim(objInfo2[0]));
			extra = ObstaclesInfo[i][O_RADIUS] = strtodbl(trim(objInfo2[1]));
		} else{
			ObstaclesInfo[i][O_MOVE] = strtodbl(trim(objInfo[4]));	
		}

        while(rectanglesOverlap(playerPos, Obstacles[i])) {
			Obstacles[i].left += (long)(45 + extra);
			Obstacles[i].right += (long)(45 + extra);

		}

        if(ObstaclesInfo[i][O_TYPE] == 3) {
			ObstaclesInfo[i][O_STEP] = 0;
			++i;
			continue;
		}

        ObstaclesInfo[i][O_STEP] = strtodbl(trim(objInfo[5], " \t)"));			

		i++;
    }
}

// Obstacles moving funcitons
int moveUpAndDown(RECT &Rect, RECT border, int step, int id, RECT Obstacles[], double ObstaclesInfo[][100], int reset = 0){
    static double info[100];
    if(reset){
        fill_n(info, 100, 0);
        return 0;
    }

    if(info[id] == 0){
        if(Rect.bottom + step >= border.bottom){
            Rect.top += border.bottom - Rect.bottom;
            Rect.bottom = border.bottom;
            info[id] = 0;
        } else {
            Rect.top -= step;
            Rect.bottom -= step;
        }
    }

    for (int i = 0; i < 100; ++i){
        if(ObstaclesInfo[i][O_TYPE] == 3 && rectanglesOverlap(Rect, Obstacles[i])){
            if(!info[id]){
                Rect.top = Obstacles[i].top - (Rect.bottom -Rect.top);
                Rect.bottom = Obstacles[i].top;
            } else {
                Rect.bottom = Obstacles[i].bottom + (Rect.bottom - Rect.top);
                Rect.top = Obstacles[i].bottom;
            }

            info[id] = !info[id];
            break;
        }
    }
    return 0;
}

int moveRightAndLeft(RECT &Rect, RECT border, int step, int id, RECT Obstacles[], double ObstaclesInfo[][100], int reset = 0){
    static double info[100];
    if(reset){
        fill_n(info, 100, 0);
        return 0;
    }

    if(info[id] == 0){
        if(Rect.right + step > border.right){
            Rect.right = border.right;
            Rect.left += border.right - Rect.right;
            info[id] = 1;
        } else { 
            Rect.right -= step;
            Rect.left -= step;
        }
    }

    for(int i = 0; i < 100; ++i){
        if(ObstaclesInfo[i][O_TYPE] == 3 && rectanglesOverlap(Rect, Obstacles[i])){
            if (!info[id]) {
                Rect.right = Obstacles[i].left;
                Rect.left = Obstacles[i].left - (Rect.right - Rect.left);
            } else {
                Rect.right = Obstacles[i].right + (Rect.right - Rect.left);
                Rect.left = Obstacles[i].right;
            }

            info[id] = !info[id];
            break;
        }
    }
    return 0;
}

int moveInCircle(RECT &Rect, RECT border, double step, double radius, int id, RECT Obstacles[], double ObstaclesInfo[][100], int reset = 0) { 
    static double info[100][10] = {};

    if(reset == 1){
        for(int i = 0; i < 100; ++i){
            fill_n(info[i], 10, 0);
        }
        return 0;
    }

    step = step/20;

    if(!info[id][0]){
        info[id][0] = Rect.left;
        info[id][1] = Rect.right;
        info[id][2] = Rect.top;
        info[id][3] = Rect.bottom;
        info[id][4] = 0;
        info[id][5] = 1;
    }

    info[id][4] = fmod(info[id][4], 360);

    RECT prev = Rect;

    Rect.left = (long)(info[id][0] + radius * cos(info[id][4]));
	Rect.right = (long)(info[id][1] + radius * cos(info[id][4]));
	Rect.top = (long)(info[id][2] + radius * sin(info[id][4]));
	Rect.bottom = (long)(info[id][3] + radius * sin(info[id][4]));

    if(Rect.left < border.left) {
		Rect.right += border.left - Rect.left;
		Rect.left = border.left;
		info[id][5] = !info[id][5];
	}

	if(Rect.top < border.top) {
		Rect.bottom += border.top - Rect.top;
		Rect.top = border.top;
		info[id][5] = !info[id][5];
	}
	if(Rect.right > border.right) {
		Rect.left -= Rect.right - border.right;
		Rect.right = border.right;
		info[id][5] = !info[id][5];
	}
	if(Rect.bottom > border.bottom) {
		Rect.top -= Rect.bottom - border.bottom;
		Rect.bottom = border.bottom;
		info[id][5] = !info[id][5];
	}

    int h = Rect.bottom - Rect.top, w = Rect.right - Rect.left;

    for(int i = 0; i < 100; ++i){
        if(ObstaclesInfo[i][O_TYPE] == 3 && rectanglesOverlap(Rect, Obstacles[i])) {
			info[id][5] = !info[id][5];
			// RL
			if( prev.left >= Obstacles[i].right ) {
			
				Rect.right = Obstacles[i].right + w; 
				Rect.left = Obstacles[i].right;
			} else
			// BT
			if( prev.top >= Obstacles[i].bottom ) {
				
				Rect.bottom = Obstacles[i].bottom + h; 
				Rect.top = Obstacles[i].bottom;
			} else
			// LR
			if( prev.right <= Obstacles[i].left ) {
				
				Rect.left = Obstacles[i].left - w; 
				Rect.right = Obstacles[i].left;
			} else
			// TB
			if( prev.bottom <= Obstacles[i].top ) {
				
				Rect.top = Obstacles[i].top - h; 
				Rect.bottom = Obstacles[i].top;
			}
			//break;
		}
    }

    if (info[id][5]){
        info[id][4] += step;
    } else {
        info[id][4] -= step;
    }
    return 0;

}

int moveBounceOnWalls(RECT &Rect, RECT border, int step, int id, RECT Obstacles[], double ObstaclesInfo[][100], int reset = 0){
    static int info[100][5];
    if(!info[id][0]){
        info[id][0] = 1;
        info[id][1] = 1;
        //info[id][2] = 0;
    } 
    if(reset){
        fill_n(info, 100, 0);
        return 0;
    }

    RECT prev = Rect;

    Rect.left += step * info[id][0];
	Rect.top += step * info[id][1];
	Rect.right += step * info[id][0];
	Rect.bottom += step * info[id][1];

	int h = Rect.bottom - Rect.top, w = Rect.right - Rect.left;

	if(Rect.top < border.top) {
		Rect.bottom = border.top + h;
		Rect.top = border.top;
		info[id][1] = 1;
	}

	if(Rect.bottom > border.bottom) {
		Rect.top = border.bottom - h;
		Rect.bottom = border.bottom;
		info[id][1] = -1;

	}

	if(Rect.left < border.left) {
		Rect.right = border.left + w;
		Rect.left = border.left;
		info[id][0] = 1;
	}

	if(Rect.right > border.right) {
		Rect.left = border.right - w;
		Rect.right = border.right;
		info[id][0] = -1;
	}

    for(int i = 0; i < 100; ++i){
        if(ObstaclesInfo[i][O_TYPE] == 3 && rectanglesOverlap(Rect, Obstacles[i])) {
			// RL
			if( prev.left >= Obstacles[i].right ) {
				info[id][0] = 1;
				Rect.right = Obstacles[i].right + w; 
				Rect.left = Obstacles[i].right;
			} else
			// BT
			if( prev.top >= Obstacles[i].bottom ) {
				info[id][1] = 1;
				Rect.bottom = Obstacles[i].bottom + h; 
				Rect.top = Obstacles[i].bottom;
			} else
			// LR
			if( prev.right <= Obstacles[i].left ) {
				info[id][0] = -1;
				Rect.left = Obstacles[i].left - w; 
				Rect.right = Obstacles[i].left;
			} else
			// TB
			if( prev.bottom <= Obstacles[i].top ) {
				info[id][1] = -1;
				Rect.top = Obstacles[i].top - h; 
				Rect.bottom = Obstacles[i].top;
			}
			//break;
		}
    }
    return 1;
}

// Helper for creating custom ellipse obstacle
void CreateCustomEllipse(HDC hdc, RECT obstacle){
    Ellipse(hdc, obstacle.left, obstacle.top, obstacle.right, obstacle.bottom);
}

// Check if rectangles overlapping with each other
int rectanglesOverlap(RECT a, RECT b){
    if(a.left >= b.right || a.right <= b.left || a.top >= b.bottom || a.bottom <= b.top)
        return 0;
    return 1;
}

// Check if player goes out of game box window
bool outOfGameBox(RECT &curPlayerPos, RECT gameBox, int direction, int step){
    if (direction == 0 && (curPlayerPos.left - step < gameBox.left)){
        curPlayerPos.right = gameBox.left + (curPlayerPos.right - curPlayerPos.left);
        curPlayerPos.left = gameBox.left;
        return true;
    }
    else if (direction == 1 && (curPlayerPos.right + step > gameBox.right)){
        curPlayerPos.left = gameBox.right - (curPlayerPos.right - curPlayerPos.left);
        curPlayerPos.right = gameBox.right;
        return true;
    }
    else if (direction == 2 && (curPlayerPos.top - step < gameBox.top)){
        curPlayerPos.bottom = gameBox.top + (curPlayerPos.bottom - curPlayerPos.top);
        curPlayerPos.top = gameBox.top;
        return true;
    }
    else if (direction == 3 && (curPlayerPos.bottom + step > gameBox.bottom)){
        curPlayerPos.top = gameBox.bottom - (curPlayerPos.bottom - curPlayerPos.top);
        curPlayerPos.bottom = gameBox.bottom;
        return true;
    }
    return false;
}

// Files manipulation
BOOL directoryExists(const std::string& dir_name){
    DWORD file_attr = GetFileAttributesA(dir_name.c_str());
    if (file_attr == INVALID_FILE_ATTRIBUTES)
        return false;
    if (file_attr & FILE_ATTRIBUTE_DIRECTORY)
        return true;

    return false;
}

BOOL fileExists(LPCTSTR path){
    DWORD file_attr = GetFileAttributes(path);

    return (file_attr != INVALID_FILE_ATTRIBUTES && !(file_attr & FILE_ATTRIBUTE_DIRECTORY));

}

// String manipulation
vector <string> splitString(const string& str, const char& ch) {
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

string trim(const string& str, const string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

double strtodbl(string String) { 
	return ::atof(String.c_str());
}

// Game flow
int game_over(HWND hwnd, RECT &mainPlayerPos, RECT &obstacle, RECT playerPos){
    if (mainPlayerPos.right > obstacle.left && mainPlayerPos.left < obstacle.right && rectanglesOverlap(mainPlayerPos, obstacle)){
        KillTimer(hwnd, 1);

        MessageBox(hwnd, TEXT("Try Again?"), TEXT("Game Over"), MB_OK);

        SetTimer(hwnd, 1, 50, NULL);

        mainPlayerPos = playerPos;

        while(rectanglesOverlap(playerPos, obstacle)){
            obstacle.left -= 4500;
            obstacle.right -= 4500;
        }

        InvalidateRect(hwnd, NULL, false);
        UpdateWindow(hwnd);
        return 1;
    }
    return 0;
}