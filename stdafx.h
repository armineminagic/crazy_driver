#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <direct.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <CommCtrl.h>

#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT 480

#define UPDATE_RATE 50

#define GAME_TITLE "Crazy box"
#define OBIECTIV "\n Drive trough the gate"
#define LEVEL_TXT "\n\n\n        LEVEL %d"
#define DEATHS "\n\n\nDEATHS %d       "
#define WIN_TXT "Level compleeted!\n\n Press ok to get to the next level !"
#define WIN_TXT_TITLE "Congratulation"
#define LOSE_TXT "Try again!"
#define LOSE_TXT_TITLE "You are looser"
#define FINISH_GAME "CONGRATULATIONS!\n\nYou are the winner!\n\n"
#define BTN_CREATE_LEVEL "Create Level"
#define BTN_EXIT "Exit"
#define ARE_YOU_SURE "Are you sure?"
#define CONFIRM "Confirm"
#define TIP_WALL "Transform the object into a wall"
#define WALL "Wall"
#define BTN_SAVE "Save"
#define BTN_RESTART "Reset"
#define BTN_BACK "Back"
#define BTN_SAVE_PROGRESS "Save Progress"
#define MSG_SUCCESS "Success"
#define MSG_PROGRESS_SAVED "The progress was saved!"
#define MSG_ERROR "ERROR"
#define MSG_OVERWRITE "This file already exists! Replace?"
#define MSG_ERROR_WRITE "There was an error while writting the file!"


// some obstacleInfo constants
#define CO_MOVE 0
#define CO_TYPE 1
#define CO_STEP 2
#define CO_RADIUS 3

// buttons
#define LOGIN_BTN 100
#define SCOREBOARD_BTN 101
#define BACK_BTN 102
#define END_BTN 103
#define RESTART_BTN 104


#define ID_BTN_CREATE 111
#define ID_BTN_WALL 112
#define ID_BTN_SAVE 113
#define ID_BTN_SAVE_PROGRESS 118

TCHAR DEFAULT_DIR_PATH[MAX_PATH] = {0};

// we include the functions
#include "colision.h"
#include "functions.h"
#include "buttons.h"
#include "movement.h"
#include "files.h"

#include "game_play.h"
#include "login.h"