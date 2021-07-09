#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h>
#include <direct.h>

#include "playgame.hpp"
#include "helpers.hpp"

#define WINDOW_WIDTH 840
#define WINDOW_HEIGHT 480

// =============================================================================================================
// SQLLITE  db HANDLER  *********************************************************************************
// =============================================================================================================

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CrazyDriver");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    // =============================================================================================================
    //INITIALIZATION SQL HANDLER 
    // =============================================================================================================


    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) (COLOR_WINDOW + 5);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;
    
    
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect( &rect, WS_OVERLAPPEDWINDOW, FALSE );

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Crazy Driver"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */ //  WS_CLIPCHILDREN|WS_OVERLAPPEDWINDOW&~WS_THICKFRAME&~WS_MAXIMIZEBOX, OVO PREGLEDATI :D
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           rect.right - rect.left,                 /* The programs width */
           rect.bottom - rect.top,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

// bool Login(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, int &ctrl, int &level);
// int ScoreTable(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, int &ctrl);

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	static int CONTROL_VARIABLE = 0; // 0 -> Login(), 1 -> PlayGame(), 2 -> ScoreBoard() 
	
    static int id = 0; // USER_ID from database 

    if(CONTROL_VARIABLE == 1) {
        if (id == 0)
        return DefWindowProc (hwnd, message, wParam, lParam);
        //  PlayGame(hwnd, message, wParam, lParam, CONTROL_VARIABLE, &db, id, level, nod); (Buttons: END) 
	} else if (CONTROL_VARIABLE == 2) { 
        //  ScoreBoard(hwnd, message, wParam, lParam, CONTROL_VARIABLE,db); (Buttons: BACK)
    } else
        //  id = Login(hwnd, message, wParam, lParam, CONTROL_VARIABLE, db); (Buttons: LOG_IN, SCORE_BOARD)
	
    return DefWindowProc (hwnd, message, wParam, lParam);
}
