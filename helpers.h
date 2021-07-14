#ifndef _HELPERS_H_
#define _HELPERS_H_

// buttons
#define BTN_END_TEXT "End"
#define BTN_END 100

#define GRACEFUL_MSG "You reach the end of the game, congrats!"

// Obstacles parameters
#define O_MOVE 0 // obstacle move
#define O_TYPE 1 // obstacle type
#define O_STEP 2
#define O_RADIUS 3 // radius

#define DIR_NAME "Levels"

TCHAR CURRENT_DIR_PATH[MAX_PATH] = {0};

int GetRandomNumber(int, int);

void PlayerMove(RECT&, RECT, int, int&);
void wallPassed(RECT &, RECT, int, RECT);

void generateGate(int, RECT &, int,  RECT);
void generateObstacles(RECT Obstacles[] , double ObstaclesInfo[][100], int Level, int margins[], RECT playerPos);

int moveUpAndDown(RECT &, RECT , int, int, RECT*, double ObstaclesInfo[][100], int);
int moveRightAndLeft(RECT &, RECT, int, int, RECT*, double ObstaclesInfo[][100], int);
int moveInCircle(RECT &, RECT, double, double, int, RECT*, double ObstaclesInfo[][100], int);
int moveBounceOnWalls(RECT &, RECT, int, int, RECT*, double ObstaclesInfo[][100], int);
void followThePlayer(RECT &, RECT, RECT, long);

void CreateCustomEllipse(HDC, RECT);
int rectanglesOverlap(RECT, RECT);
bool outOfGameBox(RECT &, RECT, int, int);

BOOL directoryExists(const std::string&);
BOOL fileExists(LPCTSTR);
void getInfo(const string&, string&, double&, double&, double&, double&, double&, double&, double&);
double strtodbl(string);

int game_over(HWND, RECT &, RECT &, RECT);


// Helper for random numbers
int GetRandomNumber(int nLow, int nHigh)
{
	srand((unsigned)time(NULL));
    return (rand() % (nHigh - nLow + 1)) + nLow;
}

//Player moving functions
void PlayerMove(RECT &curPostion, RECT gameBox, int step, int &key_ctrl){
    if (GetAsyncKeyState(VK_LEFT) && !outOfGameBox(curPostion, gameBox, 0, step)){
        cout << "VK LEFT" << endl;
        curPostion.left -= step;
        curPostion.right -= step;
        key_ctrl = 1;
    }

    if (GetAsyncKeyState(VK_RIGHT) && !outOfGameBox(curPostion, gameBox, 1, step)){
        cout << "VK RIGHT" << endl;
        curPostion.left += step;
        curPostion.right += step;
        key_ctrl = 2;
    }

    if (GetAsyncKeyState(VK_UP) && !outOfGameBox(curPostion, gameBox, 2, step)){
        cout << "VK UP" << endl;
        curPostion.top -= step;
        curPostion.bottom -= step;
        key_ctrl = 3;
    }

    if (GetAsyncKeyState(VK_DOWN) && !outOfGameBox(curPostion, gameBox, 3, step)){
        cout << "VK DOWN" << endl;
        curPostion.top += step;
        curPostion.bottom += step;
        key_ctrl = 4;
    }
}

void wallPassed(RECT &currPosition, RECT Obstacle, int dir, RECT pos) {
    if (rectanglesOverlap(currPosition, Obstacle)){
        if((dir == 3 || dir == 2) && (pos.right <= Obstacle.left))
            dir = 1;
        if ((dir == 3 || dir == 2) && (pos.left >= Obstacle.right))
            dir = 0;

        if (dir == 0 && currPosition.left < Obstacle.right) {
            currPosition.right += Obstacle.right - currPosition.left;
            currPosition.left = Obstacle.right;
        }

        if (dir == 1 && currPosition.right > Obstacle.left) {
            currPosition.left -= currPosition.right - Obstacle.left;
            currPosition.right = Obstacle.left;
        }

        if (dir == 2 && currPosition.top < Obstacle.bottom) {
            currPosition.bottom += Obstacle.bottom - currPosition.top;
            currPosition.top = Obstacle.bottom;
        }

        if (dir == 3 && currPosition.bottom > Obstacle.top) {
            currPosition.top -= currPosition.bottom - Obstacle.top;
            currPosition.bottom = Obstacle.top;
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
    moveBounceOnWalls(Obstacles[0], Obstacles[0], 1, 1, Obstacles, ObstaclesInfo, 1);
    moveInCircle(Obstacles[0], Obstacles[0], 1, 1, 1, Obstacles, ObstaclesInfo, 1);

    getcwd(CURRENT_DIR_PATH, MAX_PATH);

    if(!opendir(DIR_NAME)){
        cout << "DIRECTORY DOES NOT EXISTS " << endl;
        _mkdir(DIR_NAME);

        HANDLE hFile;

        DWORD wmWritten;
		char strData[] = "Rectangle(200,100,20,40,1,10)\r\nEllipse(150,150,50,50,2|30,5)\r";

        hFile = CreateFile("Levels/level1.txt", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        WriteFile(hFile, strData, (DWORD)(sizeof(strData)), &wmWritten, NULL);

        CloseHandle(hFile);
    }

    char filename[100];
    sprintf_s(filename, "Levels/level%d.txt", Level);
    ifstream hFile(filename);

    if(!hFile){
        return;
    }

    string line;
    int i = 0;
    double extra = 0;

    string name;
    double left, top, right, bottom, mv, radius, step;

    while (getline(hFile, line)) {
        cout << "LINE " << line << " I: " << i << endl;
        getInfo(line, name, left, top, right, bottom, mv, radius, step);
        //cout << name << " " << left << " " << top << " " << right << " " << bottom << " " << mv << " " << radius << " " << step << endl;
        if(name == "Rectangle")
            ObstaclesInfo[i][O_TYPE] = 1;
        else if (name == "Ellipse")
            ObstaclesInfo[i][O_TYPE] = 2;
        else if (name == "Wall")
            ObstaclesInfo[i][O_TYPE] = 3;
        else
            continue;

        Obstacles[i].left = (long)(margins[0] + left);
        Obstacles[i].right = (long)(Obstacles[i].left + right);
		Obstacles[i].top = (long)(margins[2] + top);
		Obstacles[i].bottom = (long)(Obstacles[i].top + bottom);

        if (radius != 0) {
			ObstaclesInfo[i][O_MOVE] = mv;
			extra = ObstaclesInfo[i][O_RADIUS] = radius;
		} else {
			ObstaclesInfo[i][O_MOVE] = mv;
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

        ObstaclesInfo[i][O_STEP] = step;
        name = "";
        left = top = right = bottom = mv = radius = step = 0;
		i++;

    }
}

// Obstacles moving functions
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
        for (int i = 100; i >= 0; --i){
            fill_n(info[i], 5, 1);
        }
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

void followThePlayer(RECT &Obstacle, RECT currPlayerPos, RECT gameBox, long step) {
    double dx = currPlayerPos.right - Obstacle.right;
    double dy = currPlayerPos.bottom - Obstacle.bottom;

    double angle = atan2(dy, dx);

    currPlayerPos.left += (long)(step * cos(angle));
    currPlayerPos.right += (long)(step * cos(angle));
    currPlayerPos.top += (long)(step * sin(angle));
    currPlayerPos.bottom += (long)(step * sin(angle));

    if (currPlayerPos.top < gameBox.top) {
        currPlayerPos.bottom += gameBox.top - currPlayerPos.top;
        currPlayerPos.top = gameBox.top;
    }

    if (currPlayerPos.bottom > gameBox.bottom) {
        currPlayerPos.top -= currPlayerPos.bottom -gameBox.bottom;
        currPlayerPos.bottom = gameBox.bottom;
    }

    if (currPlayerPos.right > gameBox.right) {
        currPlayerPos.left -= currPlayerPos.right - gameBox.right;
        currPlayerPos.right = gameBox.right;
    }

    if (currPlayerPos.left < gameBox.left) {
        currPlayerPos.right += gameBox.left - currPlayerPos.left;
        currPlayerPos.left = gameBox.left;
    }
}

// Helper for creating custom ellipse obstacle
void CreateCustomEllipse(HDC hdc, RECT obstacle){
    Ellipse(hdc, obstacle.left, obstacle.top, obstacle.right, obstacle.bottom);
}

// Check if rectangles overlapping with each other
int rectanglesOverlap(RECT a, RECT b){

    if(a.left >= b.right || a.right <= b.left || a.top >= b.bottom || a.bottom <= b.top){
        return 0;
    }
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


#endif
