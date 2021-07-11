#ifndef _PLAYGAME_HPP_
#define _PLAYGAME_HPP_

#include "helpers.hpp"

struct Player {
    int width;
    int height;

    Player(int w, int h){
        width = w;
        height = h;
    }
};

int PlayGame(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, int &ctrl /*, sqldb &, const int id, const int level, const int nod)*/){

    HDC hdc;
    static HDC hdcBuffer = {};

    PAINTSTRUCT ps;

    RECT client = {};

    static RECT gameBox = {};
    static RECT playerPos = {}; // Initial player position
    static RECT mainPlayerPos = {}; // Current player position
    static RECT previousPos = {}; // Prevous player position

    static RECT Gate = {};
    static int GateInfo[2] = {};

    static RECT Obstacles[100] = {};
    static double ObstaclesInfo[100][100] = {};

    GateInfo[0] = 15;
    GateInfo[1] = GetRandomNumber(45,65);

    static Player player = Player(45, 45);

    static HWND BUTTONS[20] = {};

    static int margins[] = {30, 35, 80, 35};
    static int width, height;

    static int step = 10;
    int key_control = 0;

    static int Level = 1;
    static int PreviousLevel = 1;

    static int PreviousDeaths = 0;
    static int Deaths = 0;

    static int TheEnd = 0;

    switch (message){
        case WM_CREATE: {

            if(!CURRENT_DIR_PATH[0])
                GetCurrentDirectory(MAX_PATH, CURRENT_DIR_PATH);

            GetClientRect(hwnd, &client);

            BUTTONS[0] = CreateWindow(TEXT("button"), TEXT(BTN_END_TEXT), WS_VISIBLE | WS_CHILD,  client.right - margins[1], client.bottom - margins[3], margins[2], 25, hwnd, (HMENU)BTN_END, NULL, NULL);

            width = client.right;
            height = client.bottom;

            hdcBuffer = CreateCompatibleDC(NULL);

            hdc = GetDC(hwnd);

            static HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height);

            SelectObject(hdcBuffer, hbitmap);

            // Postition of the player widnow
            gameBox.right = client.right - margins[1];
            gameBox.left = client.left + margins[0];
            gameBox.top = client.top + margins[2];
            gameBox.bottom = client.bottom - margins[3];

            // Position of the player
            playerPos.right = client.right + margins[0] + player.width;
            playerPos.left = client.left + margins[0];
            playerPos.top = (client.top + client.bottom)/2;
            playerPos.bottom = (client.top + client.bottom)/2 + player.height;

            generateGate(margins[2], Gate, GateInfo, gameBox);

            mainPlayerPos = playerPos;

            generateObstacles(Obstacles, ObstaclesInfo, Level, margins, playerPos);

            SetTimer(hwnd,1,50, NULL);

            ReleaseDC(hwnd, hdc);

            return 0;
        }

        case WM_PAINT: {
            hdc = BeginPaint(hwnd, &ps);

            BitBlt(hdcBuffer, 0, 0, width, height, NULL, NULL, NULL, WHITENESS);

            if (!TheEnd){

                DrawText(hdcBuffer, "CRAZY DRIVER", -1, &client, DT_CENTER);
                DrawText(hdcBuffer, "Get out trough the green door", -1, &client, DT_CENTER);

                char Text[100];
                sprintf_s(Text, LVL_TEXT, Level);
                DrawText(hdcBuffer, TEXT(Text), -1, &client, DT_LEFT);

                sprintf_s(Text, NOD_TEXT, Deaths);
                DrawText(hdcBuffer, TEXT(Text), -1, &client, DT_RIGHT);

                FillRect(hdcBuffer, &gameBox, CreateSolidBrush(RGB(189, 237, 152)));
                FrameRect(hdcBuffer, &gameBox, CreateSolidBrush(RGB(217, 221, 107)));

                FillRect(hdcBuffer, &mainPlayerPos, CreateSolidBrush(RGB(213, 76, 76)));

                FillRect(hdcBuffer, &Gate, CreateSolidBrush(RGB(102, 222, 147)));

                for (int i = 0; i < sizeof(Obstacles)/sizeof(Obstacles[0]); ++i){
                    if(ObstaclesInfo[i][O_TYPE] == 1)
                        FillRect(hdcBuffer, &Obstacles[i], CreateSolidBrush(RGB(250, 213, 134)));
                    else if(ObstaclesInfo[i][O_TYPE] == 2){
                        SelectObject(hdcBuffer, CreateSolidBrush(RGB(40, 82, 122)));
                        CreateCustomEllipse(hdcBuffer, Obstacles[i]);
                    } else if (ObstaclesInfo[i][O_TYPE] == 3){
                        if(ObstaclesInfo[i][O_MOVE] == 1) {
                            FillRect(hdcBuffer, &Obstacles[i], CreateSolidBrush(RGB(10, 25, 49)));
                        } else if (ObstaclesInfo[i][O_MOVE] == 2){
                            SelectObject(hdcBuffer, CreateSolidBrush(RGB(10, 25, 49)));
                            CreateCustomEllipse(hdcBuffer, Obstacles[i]);
                        }
                    }
                }

                BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);

                EndPaint(hwnd, &ps);

                ReleaseDC(hwnd, hdc);
                ReleaseDC(hwnd, hdcBuffer);

                for (int i = 0; i < sizeof(Obstacles)/sizeof(Obstacles[0]); ++i){
                    if(ObstaclesInfo[i][O_TYPE] != 3 && game_over(hwnd, mainPlayerPos, Obstacles[i], playerPos)){
                        generateObstacles(Obstacles, ObstaclesInfo, Level, margins, playerPos);
                        Deaths++;
                        break;
                    }
                }
            } else {
                DrawText(hdcBuffer, TEXT(GRACEFUL_MSG), -1, &client, DT_CENTER);

                BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);

                EndPaint(hwnd, &ps);
                ReleaseDC(hwnd, hdc);
                ReleaseDC(hwnd, hdcBuffer);
            }

            return 0;
        }

        case WM_KEYDOWN: {
            previousPos = mainPlayerPos;

            PlayerMove(mainPlayerPos, gameBox, step, key_control);

            if (wParam == VK_ESCAPE){
                mainPlayerPos = playerPos;
                key_control = 5;
                Deaths++;
            }

            for(int i = 0; i < sizeof(Obstacles)/sizeof(Obstacles[0]); ++i){
                if(ObstaclesInfo[i][O_TYPE] == 3){
                    wallPassed(mainPlayerPos, Obstacles[i], key_control-1, previousPos);
                }
            }

            if (key_control) {
                InvalidateRect(hwnd, NULL, false);
                UpdateWindow(hwnd);
            }

            if (mainPlayerPos.right > Gate.left &&  rectanglesOverlap(Gate, mainPlayerPos)) {
                KillTimer(hwnd, 1);;

                MessageBox(hwnd, TEXT("You won!"), TEXT("Congrats!"), MB_OK);

                char tmpText[100];
                sprintf_s(tmpText, "./LEVELS/level%d.txt", Level + 1);

                if (fileExists(tmpText)) {
                    generateGate(margins[2], Gate, GateInfo, gameBox);

                    SetTimer(hwnd, 1, 50, NULL);

                    mainPlayerPos = playerPos;

                    Level++;

                    generateObstacles(Obstacles, ObstaclesInfo, Level, margins, playerPos);
                }

                InvalidateRect(hwnd, NULL, false);
                UpdateWindow(hwnd);
            }

            return 0;
        }

        case WM_TIMER: {

            for (int i = 0; i < sizeof(Obstacles)/sizeof(Obstacles[0]); ++i) {
                if (ObstaclesInfo[i][O_TYPE] == 3)
                    continue;

                if (ObstaclesInfo[i][O_TYPE] == 1)
                    moveUpAndDown(Obstacles[i], gameBox, (long)ObstaclesInfo[i][O_STEP], i, Obstacles, ObstaclesInfo);

                else if (ObstaclesInfo[i][O_TYPE] == 2)
                    moveInCircle(Obstacles[i], gameBox, (long)ObstaclesInfo[i][O_STEP], ObstaclesInfo[i][O_RADIUS], i, Obstacles, ObstaclesInfo);

                else if (ObstaclesInfo[i][O_TYPE] == 3)
                    moveBounceOnWalls(Obstacles[i], gameBox, (long)ObstaclesInfo[i][O_STEP], i, Obstacles, ObstaclesInfo);

                else if (ObstaclesInfo[i][O_TYPE] == 4)
                    followThePlayer(Obstacles[i], mainPlayerPos, gameBox, (long)ObstaclesInfo[i][O_STEP]);

                else if (ObstaclesInfo[i][O_TYPE] == 5)
                    moveRightAndLeft(Obstacles[i], gameBox, (long)ObstaclesInfo[i][O_STEP], i, Obstacles, ObstaclesInfo);
            }

            InvalidateRect(hwnd, NULL, false);
            UpdateWindow(hwnd);
        }

        // case WM_COMMAND: {

        //     switch(wParam) {
        //         case
        //     }
        // }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return 0;
}


#endif