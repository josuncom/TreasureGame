

#pragma warning(disable:4267)
#pragma warning(disable:4996)

#include <Windows.h>
#include <conio.h>

// 색상 매크로
#define BG_RED         BACKGROUND_RED
#define FG_RED	       FOREGROUND_RED
#define BG_RED1        BACKGROUND_RED|BACKGROUND_INTENSITY
#define FG_RED1        FOREGROUND_RED|FOREGROUND_INTENSITY
#define BG_BLUE        BACKGROUND_BLUE
#define FG_BLUE	       FOREGROUND_BLUE
#define BG_BLUE1       BACKGROUND_BLUE|BACKGROUND_INTENSITY
#define FG_BLUE1       FOREGROUND_BLUE|FOREGROUND_INTENSITY
#define BG_GREEN       BACKGROUND_GREEN
#define FG_GREEN       FOREGROUND_GREEN
#define BG_GREEN1      BACKGROUND_GREEN|BACKGROUND_INTENSITY
#define FG_GREEN1      FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define BG_YELLOW      BACKGROUND_RED|BACKGROUND_GREEN                       
#define FG_YELLOW      FOREGROUND_RED|FOREGROUND_GREEN
#define BG_YELLOW1     BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_INTENSITY
#define FG_YELLOW1     FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define BG_EMERALD     BACKGROUND_BLUE|BACKGROUND_GREEN                     
#define FG_EMERALD     FOREGROUND_BLUE|FOREGROUND_GREEN
#define BG_EMERALD1    BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_INTENSITY
#define FG_EMERALD1    FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY
#define BG_GRAY	       BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE       
#define FG_GRAY	       FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE
#define BG_WHITE       BG_GRAY|BACKGROUND_INTENSITY
#define FG_WHITE       FG_GRAY|FOREGROUND_INTENSITY
#define BG_BLACK       0
#define FG_BLACK       0
#define BG_BLACK1      0|BACKGROUND_INTENSITY
#define FG_BLACK1      0|BACKGROUND_INTENSITY

// 키 ASCII code
#define ESC			27  // escape
#define LEFT		75  // 커서키 
#define RIGHT		77  // 커서키 
#define DOWN		80  // 커서키  
#define UP			72  // 커서키  
#define SPACE		32  //
#define ENTER		13  //
#define G			103 // g 키

// 눌린 키
enum PushKey { NONE = 0, MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, MOVE_UP, PUSH_ENTER, PUSH_SPACE, PUSH_G, GAME_QUIT };

HANDLE GetConsoleHandle();
int LocateCursor(HANDLE conHandle, int x, int y);
int PutAttrCharsXY(HANDLE conHandle, int x, int y, char* chs, WORD color);
void ClearScreen(HANDLE conHandle, int width, int height);
int SetCursorState(HANDLE conHandle, DWORD dwSize, int bVisible);
int  UpdateKey();

	