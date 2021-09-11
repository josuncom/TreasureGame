#include "header.h"

// 표준출력핸들 얻기
HANDLE GetConsoleHandle()
{
	HANDLE conHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	return conHandle;
}



// 출력좌표 설정
int LocateCursor(HANDLE hOutput, int x, int y)
{
	int bRval;
	COORD Cdpos;
	
	Cdpos.X = x;
	Cdpos.Y = y;

	bRval = SetConsoleCursorPosition(hOutput, Cdpos);

	return bRval;
}



// 좌표에 문자열 색상출력
int PutAttrCharsXY(HANDLE hOutput, int x, int y, char* chs, WORD color)
{
	DWORD dw;
	int bRval;

	LocateCursor(hOutput, x, y);
	bRval = SetConsoleTextAttribute(hOutput, color);
	WriteFile(hOutput, chs, strlen(chs), &dw, NULL);	//printf("%s", chs);

	return bRval;
}



// 화면 채우기
void ClearScreen(HANDLE hOutput, int width, int height)
{
	COORD Cdpos = { 0, 0 };
	DWORD dw;
	width++;	
	height++;
	FillConsoleOutputCharacter(hOutput, ' ', width*height, Cdpos, &dw);
	SetConsoleCursorPosition(hOutput, Cdpos);	
}



// 커서 모양설정
int SetCursorState(HANDLE hOutput, DWORD dwSize, int bVisible)
{
	int bRval;
	CONSOLE_CURSOR_INFO curInfo;

	curInfo.dwSize = dwSize;
	curInfo.bVisible = bVisible;

	bRval = SetConsoleCursorInfo(hOutput, &curInfo);

	return bRval;
}


int UpdateKey()
{
	enum PushKey key = NONE;

	if (kbhit()) //키보드를 눌렸나?
	{	
		switch (getch())
		{
		case LEFT:  key = MOVE_LEFT; break;
		case RIGHT: key = MOVE_RIGHT; break;
		case DOWN: 	key = MOVE_DOWN; break;
		case UP:	key = MOVE_UP;	break;
		case ESC:   key = GAME_QUIT; break;
		case ENTER: key = PUSH_ENTER; break;
		case SPACE: key = PUSH_SPACE; break;
		case G:     key = PUSH_G; break;
		}
	}

	return key;
}
