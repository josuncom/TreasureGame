#pragma warning(disable:4996)

#include <stdio.h>
#include <time.h>
#include "header.h"

#define ITEM_COUNT_MAX	6
#define MAX_STRING		100
#define DEFAULT_NAME	"플레이어"
#define MAX_RANKER		5
#define WAIT_TIME		1500

#define MAP_SIZE_X		60
#define MAP_SIZE_Y		24
#define PLAYER_BEGIN_X	2
#define PLAYER_BEGIN_Y  (MAP_SIZE_Y - 1)

// 아이템 종류
enum ItemKind { NoItem = -1, Mine = 0, Treasure, Lucky, InitPos, InitFlag, InitScore };

// 플레이어 구조체
typedef struct tagPlayer
{	
	char* draw;
	WORD color;

	char name[MAX_STRING];
	int	x;
	int	y;	
	int oldX;
	int oldY;
	BOOL live;		// 생존 여부	
	int	score;		// 보물 획득 누적 점수
	int treasure;	// 이번판 보물 점수	
	int	timer;		// 남은 타이머
	int	distance;	// 이동 횟수(거리)	
} Player;

// 아이템 구조체
typedef struct tagItem
{	
	char* draw;
	char* drawGod;	// god time일때 보여지는 모습
	WORD color;

	int	x;
	int	y;	
	BOOL visible;	// 보이는지 여부(유효 아이템)
	enum ItemKind kind;
} Item;

// 랭킹 구조체
typedef struct tagRanker
{
	char	name[MAX_STRING];
	int		score;
} Ranker;


BOOL GodTime = FALSE;	// 깃발에 숨겨진 아이템을 보여준다
Player player;
Item item[100];
int itemCount = 0;	// 생성된 아이템의 수
int maxTreasure;	// 이번판의 보물의 수
int level = 0;		// 레벨
int levelTimer[] = { 30, 40, 50 };	// 레벨의 타이머

// 랭커를 +1, 현재 플레이어를 마지막에 넣어 정렬후 5명만 출력
// 기본 랭커 (ranker.txt가 존재하면 바뀐다)
Ranker ranker[MAX_RANKER + 1] = {
	{"이것은", 2 },
	{"기본으로", 1 },
	{"설정된", 1 },
	{"랭킹", 1 },
	{"입니다.", 1 },
	{"empty", 0 },
};

// 플레이어 생성
void CreatePlayer(Player* play, char* draw, WORD color, int x, int y, int timer)
{	
	play->draw = draw;
	play->color = color;
	play->x = x;
	play->y = y;	
	play->timer = timer;
	play->live = TRUE;
	play->score = 0;
	play->treasure = 0;
	play->distance = 0;
}

// 아이템 생성
void CreateItem(Item* it, char* draw, char* drawGod, WORD color, int x, int y, enum ItemKind kind)
{
	it->draw = draw;
	it->drawGod = drawGod;
	it->color = color;
	it->x = x;
	it->y = y;
	it->visible = TRUE;
	it->kind = kind;
}

// 자기 아이템이 아니고, 기존의 아이템과 좌표 또는 플레이어의 좌표와 같으면 TRUE리턴
BOOL IsEqualPos(int x, int y, int index)
{
	for (int i = 0; i < itemCount; i++)
	{		
		if (i != index && 
			((item[i].x == x && item[i].y == y) || (player.x == x && player.y == y)))
			return TRUE;
	}

	return FALSE;
}

// 레벨 생성
void CreateLevel(int level)
{
	int i, x, y;
	WORD color;
	enum ItemKind kind[] = { Mine, Treasure, Lucky, InitPos, InitFlag, InitScore };
	int count[3][6] = { { 5, 3, 2, 2, 1, 1}, { 9, 3, 2, 3, 2, 1}, { 18, 3, 2, 4, 2, 1 } };	// 3개의 레벨에 ItemKind순으로 생성되는 아이템의 count
	char* hidden[] =  { "⊙", "▶", "▶", "▶", "▶", "▶" };		// 숨겨진 아이템의 깃발 모습
	char* visible[] = { "⊙", "▦", "Lk", "IP", "IF", "IS" }; 	// 보이는 아이템 	
		
	i = 0;
	// index kind
	for (int ik = 0; ik < 6; ik++)	
	{
		// 보물이면 숨겨진 보물의 수를 저장한다
		if (kind[ik] == Treasure)
			maxTreasure = count[level][ik];

		// 아이템 종류별로 count만큼 생성
		// index count
		for (int ic = 0; ic < count[level][ik]; ic++)
		{		
			// 중복되지 않는 좌표를 얻는다
			do
			{				
				x = rand() % ((MAP_SIZE_X - 4) / 2) + 1; // 1 ~ 29
				x *= 2; // 2 ~ 58
				y = rand() % (MAP_SIZE_Y - 1) + 1; // 1 ~ 19;

			} while (IsEqualPos(x, y, i));

			if (kind[ik] == Mine)
				color = BG_BLACK | FG_EMERALD1; // 지뢰색
			else
				color = BG_BLACK | FG_BLUE1;	// 깃발색
			CreateItem(&item[i], hidden[ik], visible[ik], color, x, y, kind[ik]);
			i++;
		}
	}

	itemCount = i;
}

// 깃발의 위치를 새롭게 바꾼다
void SetFlagNewPos()
{
	int x, y;

	for (int i = 0; i < itemCount; i++)
	{
		// 보이는 아이템이고 폭탄이 아닌 모든 아이템의 좌표를 바꾼다
		if (item[i].visible && item[i].kind != Mine)
		{
			do
			{
				x = rand() % ((MAP_SIZE_X - 4) / 2) + 1; // 1 ~ 29
				x *= 2; // 2 ~ 58
				y = rand() % (MAP_SIZE_Y - 1) + 1; // 1 ~ 19

			} while (IsEqualPos(x, y, i));

			item[i].x = x;
			item[i].y = y;
		}
	}
}

// 맵 테두리 그리기
void DrawMapFrame(HANDLE con)
{
	int ix, iy;
	
	// 가로
	for (ix = 0; ix <= 76; ix+=2)
	{
		PutAttrCharsXY(con, ix+2, 0, "■", FG_GRAY);
		PutAttrCharsXY(con, ix+2, MAP_SIZE_Y, "■", FG_GRAY);
	}	

	// 세로
	for (iy = 0; iy < MAP_SIZE_Y+1; iy++)
	{
		PutAttrCharsXY(con, 0, iy, "■",  FG_GRAY);
		PutAttrCharsXY(con, MAP_SIZE_X, iy, "■",  FG_GRAY);
	}
}

// 아이템 그리기
void DrawItems(HANDLE con)
{
	char* draw;
	for (int i = 0; i < itemCount; i++)
	{
		if (item[i].visible)
		{
			draw = GodTime ? item[i].drawGod : item[i].draw;
			PutAttrCharsXY(con, item[i].x, item[i].y, draw, item[i].color);
		}
	}
}

// 게임 메시지(key는 키입력 대기여부)
int GameMessage(HANDLE con, char* text, BOOL Key)
{
	size_t textLen = strlen(text);
	int x = (MAP_SIZE_X - textLen) / 2;
	PutAttrCharsXY(con, x, MAP_SIZE_Y / 2, text, FG_YELLOW1);

	// Space키를 누를때까지 반복 체크
	if (Key) while (UpdateKey() != PUSH_SPACE) Sleep(50);
	
	return x + textLen; // 출력 마지막 좌표 (이름입력에서 사용)
}

// 벌칙 깃발을 하나 제거한다
BOOL RemovePenaltyFlag()
{
	BOOL success = FALSE;

	for (int i = 0; i < itemCount; i++)
	{
		if (item[i].visible &&
			(item[i].kind == InitPos || item[i].kind == InitFlag || item[i].kind == InitScore))
		{
			item[i].visible = FALSE;
			success = TRUE;
			break;
		}
	}

	return success;
}


// 플레이어 이동 및 아이템 체크
enum itemKind UpdatePlayer(int key)
{	
	enum ItemKind kind = NoItem;
	HANDLE con;

	// 이전 좌표
	player.oldX = player.x;
	player.oldY = player.y;

	switch (key)
	{
	case MOVE_LEFT:	 player.x -= 2; break;
	case MOVE_RIGHT: player.x += 2; break;
	case MOVE_DOWN:	 player.y++; break;
	case MOVE_UP:	 player.y--; break;
	}

	// 맵영역 밖이면
	if (player.x < 2 || player.x > MAP_SIZE_X - 2 ||
		player.y < 1 || player.y > MAP_SIZE_Y - 1)
	{
		player.x = player.oldX;
		player.y = player.oldY;

		return kind;
	}

	con = GetConsoleHandle();

	// 이전좌표와 다르면 이동거리 증가
	if (player.oldX != player.x || player.oldY != player.y) player.distance++;

	// 아이템 충돌 체크
	for (int i = 0; i < itemCount; i++)
	{
		// 좌표가 같고 아이템이 보이는 상태면
		if (player.x == item[i].x && player.y == item[i].y && item[i].visible)
		{
			switch (item[i].kind)
			{
			case Mine: 
				GameMessage(con, "『 지뢰를 밟았네요 』", TRUE);
				player.live = FALSE; 
				item[i].visible = FALSE;				
				ClearScreen(con, 80, 25);
				return Mine;
			case Treasure:				
				player.treasure++;
				player.score++;
				if (player.treasure == maxTreasure)
					GameMessage(con, "『 보물을 모두 찾았네요! 다음 레벨로 갑니다 』", TRUE);
				else
					GameMessage(con, "『 보물을 찾았네요! 』", TRUE);
				item[i].visible = FALSE;
				ClearScreen(con, 80, 25);
				break;
			case Lucky: // 이득이 되는 아이템				    
				if (RemovePenaltyFlag())
					GameMessage(con, "『 행운의 아이템입니다! 벌칙 깃발을 1개 제거합니다  』", TRUE);
				else
					GameMessage(con, "『 행운의 아이템입니다! 벌칙 깃발이 없네요 』", TRUE);
				item[i].visible = FALSE;
				ClearScreen(con, 80, 25);
				break;
			case InitPos:
				GameMessage(con, "『 출발 위치로 이동 합니다. 』", TRUE);
				player.x = PLAYER_BEGIN_X;
				player.y = PLAYER_BEGIN_Y;
				item[i].visible = FALSE;
				ClearScreen(con, 80, 25);
				break;
			case InitFlag:
				GameMessage(con, "『 깃발의 위치를 바꿉니다. 』", TRUE);
				item[i].visible = FALSE;				
				ClearScreen(con, 80, 25);
				break;
			case InitScore:
				GameMessage(con, "『 점수를 초기화 합니다. 』", TRUE);
				item[i].visible = FALSE;
				ClearScreen(con, 80, 25);
				break;
			}

			kind = item[i].kind;
		}
	}	

	return kind;
}

// 게임 타이틀 화면 출력
int GameTitle()
{
	int select = 1;
	enum PushKey key;
	int y = 6, old_y = 0;	
	
	// 콘솔 출력 핸들
	HANDLE con = GetConsoleHandle();

	while(1)
	{
		PutAttrCharsXY(con, 32, old_y, "               ", BG_BLACK | FG_BLUE1);	// 지우고
		PutAttrCharsXY(con, 32, y, "▷           ◁", BG_BLACK | FG_BLUE1);		// 출력

		PutAttrCharsXY(con, 35, 6,  "초     급", BG_BLACK | FG_WHITE);
		PutAttrCharsXY(con, 35, 8,  "중     급", BG_BLACK | FG_WHITE);
		PutAttrCharsXY(con, 35, 10, "고     급", BG_BLACK | FG_WHITE);
		PutAttrCharsXY(con, 35, 12, "이름 등록", BG_BLACK | FG_WHITE);
		PutAttrCharsXY(con, 35, 14, "랭킹 보기", BG_BLACK | FG_WHITE);
		PutAttrCharsXY(con, 35, 16, "게임 종료", BG_BLACK | FG_WHITE);				

		old_y = y;

		key = UpdateKey();
		if (key == MOVE_UP) y -= 2;
		if (key == MOVE_DOWN) y += 2;
		if (key == PUSH_SPACE) break;

		if (y < 6)
			y = 16;
		else if (y > 16)	
			y = 6;	

		Sleep(20);
	}
		
	// 메뉴 선택
	switch (y)
	{
	case 7 : select = 1; break;
	case 8:  select = 2; break;
	case 10: select = 3; break;
	case 12: select = 4; break;
	case 14: select = 5; break;
	case 16: select = 6;
	}

	return select;
}

void InputPlayerName(HANDLE con)
{
	int x;

	// 커서 보임
	SetCursorState(con, 1, 1);

	ClearScreen(con, 80, 25);

	x = GameMessage(con, "플레이어 이름 : ", FALSE); // 키 입력대기 없음

	LocateCursor(con, x, MAP_SIZE_Y / 2);
	scanf("%s", player.name);

	// 커서 숨김
	SetCursorState(con, 1, 0);

}

// 게임 랭킹 출력
void GameRanking(HANDLE con)
{
	char text[MAX_STRING];

	ClearScreen(con, 80, 25);
	PutAttrCharsXY(con, 33, 4, "☆ 게임  랭킹 ☆", BG_BLACK | FG_GREEN1);

	for (int iy = 8, i = 0; i < 5; iy += 2, i++)
	{
		sprintf(text, "%d. %-8s %03d", i+1, ranker[i].name, ranker[i].score);
		PutAttrCharsXY(con, 33, iy, text, BG_BLACK | FG_WHITE);
	}	

	// Space키를 누를때까지 반복 체크
	while (UpdateKey() != PUSH_SPACE) Sleep(50);	
}

// 순위 구하기(버블 정렬)
void SortRanker()
{
	int i, j;
	Ranker temp;

	// 내림차순
	for (i = 0; i < MAX_RANKER+1; i++)
	{
		for (j = 0; j < MAX_RANKER; j++)
		{
			if (ranker[j].score < ranker[j + 1].score)		
			{
				// 교환
				temp          = ranker[j];
				ranker[j]     = ranker[j + 1];
				ranker[j + 1] = temp;
			}
		}
	}
}

// 랭커파일 읽기
void LoadRanker(char* file)
{
	int i;
	FILE* fp;	

	fp = fopen(file, "rt");			// 파일 읽기 개방
	if (fp)
	{
		// 랭커 만큼 읽는다
		for (i = 0; i < MAX_RANKER; i++)
		{
			fscanf(fp, "%s %d", ranker[i].name, &(ranker[i].score));
		}
		fclose(fp);
	}
}

// 랭커파일 쓰기
void SaveRanker(char* file)
{
	int i;
	FILE* fp;

	fp = fopen(file, "wt");			// 파일 쓰기 개방
	if (fp)
	{
		// 랭커 만큼 쓴다
		for (i = 0; i < MAX_RANKER; i++)
		{
			fprintf(fp, "%s %d\n", ranker[i].name, ranker[i].score);
		}
		fclose(fp);
	}
}

// 게임 플레이 
void GameLoop(HANDLE con)
{	
	char text[MAX_STRING];
	clock_t beginClock;			// 시간체크를 위한 변수
	int second = 0;
	enum PushKey key;
	enum itemKind kind;
	
	GodTime = FALSE;
	ClearScreen(con, 80, 25);	

	beginClock = clock();

	while (1)
	{
		player.oldX = player.x;
		player.oldY = player.y;

		// 입력 및 좌표 갱신
		key = UpdateKey();
		if (key == PUSH_G) GodTime = !GodTime;
		if (key)
		{
			kind = UpdatePlayer(key);
			if (kind == InitFlag)  // 깃발 위치 초기화(깃발의 위치를 새롭게 설정)
				SetFlagNewPos();
			if (kind == InitScore) // 점수 초기화
			{ 
				// 점수가 보물 획득수이므로 현상태에서 새로 게임을 시작한다
				player.x = PLAYER_BEGIN_X;
				player.y = PLAYER_BEGIN_Y;
				player.timer = levelTimer[level];
				player.treasure = 0;				
				CreateLevel(level);
			}

			if (player.live == FALSE) break; // 사망
			
			// 보물을 모두 찾았으면
			if (player.treasure == maxTreasure)
			{
				// 2렙까지는 레벨업, 3렙은 계속 3렙 유지
				if (level < 2) level++;

				// 초기화
				player.x = PLAYER_BEGIN_X;
				player.y = PLAYER_BEGIN_Y;
				player.treasure = 0;
				player.timer = levelTimer[level];

				// 새로운 게임을 시작한다 (레벨3은 레벨3으로 계속)
				CreateLevel(level);				
			}
		}

		// 맵 테두리 출력
		DrawMapFrame(con);

		// 아이템 출력
		DrawItems(con);

		// 플레이어 지우기
		PutAttrCharsXY(con, player.oldX, player.oldY, "  ", BG_BLACK | FG_GREEN1);

		// 플레이어 출력
		PutAttrCharsXY(con, player.x, player.y, player.draw, BG_BLACK | FG_GREEN1);

		// 현재시간 구하기 (1/1000초:밀리초)		
		second = (clock() - beginClock) / CLOCKS_PER_SEC; // 초단위로 변환
		if (second >= 1) // 1초 이상이면 타이머 감소(게임메시지 대기시간은 몇초가 되든 무시함)
		{
			beginClock = clock();
			player.timer--;
		}
		
		// 게임 정보 출력
		sprintf(text, "이름 : %s", player.name);				
		PutAttrCharsXY(con, 64, 1, text, BG_BLACK | FG_WHITE);
		sprintf(text, "레벨 : %d", level+1);				
		PutAttrCharsXY(con, 64, 2, text, BG_BLACK | FG_WHITE);		
		sprintf(text, "총 점 수 : %d", player.score);		
		PutAttrCharsXY(con, 64, 4, text, BG_BLACK | FG_WHITE);
		sprintf(text, "찾은보물 : %d", player.treasure);		
		PutAttrCharsXY(con, 64, 5, text, BG_BLACK | FG_WHITE);
		sprintf(text, "시    간 : %-3d", player.timer);		
		PutAttrCharsXY(con, 64, 6, text, BG_BLACK | FG_WHITE);		
		sprintf(text, "이동횟수 : %-3d", player.distance);	
		PutAttrCharsXY(con, 64, 7, text, BG_BLACK | FG_WHITE);

		if (player.timer <= 0)
		{
			GameMessage(con, "『 타임오버 』", TRUE);
			ClearScreen(con, 80, 25);
			break;
		}

		Sleep(30); // 지연시간
	}	
}

int main()
{	
	int select;
	HANDLE con;

	// 콘솔 화면핸들 얻기
	con = GetConsoleHandle();

	// 커서 숨김
	SetCursorState(con, 1, 0);

	// 난수를 time으로 결정
	srand((unsigned int)time(0));	

	// 플레이어 이름을 기본이름으로 설정
	strcpy(player.name, DEFAULT_NAME);

	// 랭커 파일 읽기
	LoadRanker("ranker.txt");

	// 타이틀 화면
	while(1)
	{
		ClearScreen(con, 80, 25);				

		select = GameTitle();
		if (select >= 1 && select <= 3)
		{
			level = select - 1;
		}
		else if (select == 4)
		{
			InputPlayerName(con);
			continue;
		}
		else if (select == 5)
		{
			GameRanking(con);
			continue;
		}
		else break; // 종료

		// 레벨 선택이면
		if (level >= 0 && level <= 2)
		{
			CreatePlayer(&player, "◆", BG_BLACK | FG_GREEN, PLAYER_BEGIN_X, PLAYER_BEGIN_Y, levelTimer[level]);
			CreateLevel(level);
			GameLoop(con); // 사망후 함수를 빠져나온다
			
			// 이름 및 점수를 ranker 마지막(5번 index)에 넣는다
			strcpy(ranker[MAX_RANKER].name, player.name);
			ranker[MAX_RANKER].score = player.score;

			// 점수 정렬 및 랭킹 출력
			SortRanker();
			GameRanking(con);
		}
	}	

	// 랭커 파일 쓰기
	SaveRanker("ranker.txt");

	return 0;
}