#include "header.h"

// ǥ������ڵ� ���
HANDLE GetConsoleHandle()
{
	HANDLE conHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	return conHandle;
}



// �����ǥ ����
int LocateCursor(HANDLE hOutput, int x, int y)
{
	int bRval;
	COORD Cdpos;
	
	Cdpos.X = x;
	Cdpos.Y = y;

	bRval = SetConsoleCursorPosition(hOutput, Cdpos);

	return bRval;
}



// ��ǥ�� ���ڿ� �������
int PutAttrCharsXY(HANDLE hOutput, int x, int y, char* chs, WORD color)
{
	DWORD dw;
	int bRval;

	LocateCursor(hOutput, x, y);
	bRval = SetConsoleTextAttribute(hOutput, color);
	WriteFile(hOutput, chs, strlen(chs), &dw, NULL);	//printf("%s", chs);

	return bRval;
}



// ȭ�� ä���
void ClearScreen(HANDLE hOutput, int width, int height)
{
	COORD Cdpos = { 0, 0 };
	DWORD dw;
	width++;	
	height++;
	FillConsoleOutputCharacter(hOutput, ' ', width*height, Cdpos, &dw);
	SetConsoleCursorPosition(hOutput, Cdpos);	
}



// Ŀ�� ��缳��
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

	if (kbhit()) //Ű���带 ���ȳ�?
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
