#include<stdio.h>
#include<Windows.h>
#include<math.h>
#include<time.h>
#include<conio.h>
#include <mmsystem.h>


#pragma comment(lib,"winmm.lib")



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//소프트 웨어 설계기초 코드 (17.12.04 일) 이영호 수정본



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32

COORD pt;
int hide = 0;
int GamePlayBoard[20][40];
int Original_GamePlayBoard[20][40];

int flag = 0;


typedef struct info
{
	int life;
	int time;
	int score;
	int original_score;
}info;
info Information;



struct  npc
{
	int npc_start_x; // x좌표 왼쪽
	int npc_destination_x; // x좌표 오른쪽
	int npc_start_y; // 오른쪽 y좌표
	int npc_destination_y; // 오른쪽 y좌표
	int vector; // 벡터(방향값)
	COORD current_pos; // 현재 위치
	COORD original_start;
	COORD original_destination;
	int upflag;
	int downflag;
	COORD npc_temp_pos;
	int flag_count;

};
struct npc **my_npc;
struct turret
{
	int start_x;
	int finish_x;
	int pos_y;
	int vector;
	COORD current_pos;
};
struct turret **my_turret;

int total_npc_count = 8;
int current_npc_count = 3;
int total_turret_count = 3;
int current_turret_count = 1;

int total_stage_num = 4;
int current_stage_num = 0;
int NPC_Speed = 200;

//////////
int sleep_time = 20;
int te = 0;;
int hide_time = 0;


/////////////////////////// 새로 생성부분///////

COORD client_cur_pos;

boolean item_is_taken;
boolean key_is_taken;

//////////////////////////////////////////////

void RemoveCursor();
void SetCurrentCursorPos(int x, int y);
COORD GetCurrentCursorPos();

void initInformation();

void manualBoard();
void SetGameBoard();
void SetGameBoard2();
void SetGameBoard3();
void SetInformation();
void GameClear();
void GameOver();
void DrawSetGameBoard(int GamePlayBoard[][40]);
void Ladder(int x, int y);
void ProhibitHideArea(int x_s, int y_s, int x_d, int y_d);

void ProcessKeyInput();

void DrawCharacter();
void DeleteCharacter();
void HideCharacter();

int ShiftRight();
int ShiftLeft();
int ShiftUp();
int ShiftDown();
int Jump();
void Hide();

void set_npc();
int move_npc();
void set_turret();
void move_turret();

int DetectCollision(int posX, int posY);
int Check_Collition();
void item_Switch();
void item_Coin();

void stage3_Event();





void RemoveCursor()
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	pt.X = x;
	pt.Y = y;
}
COORD GetCurrentCursorPos()
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}

void initInformation()
{
	Information.life = 3;
	Information.score = 0;
	Information.time = 30;
	Information.original_score = 0;
	return;
}

void manualBoard()
{
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
	{
		SetCurrentCursorPos(x * 2, y);


		if (y == 1 || y == 19)
		{
			printf("■");
		}
		else if (x == 0 && y != 0 || x == 39 && y != 0)
		{
			printf("■");
		}
		else
		{
			printf("  ");
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);

	SetCurrentCursorPos(10, 3);
	printf("송박이의 Prison Break");
	SetCurrentCursorPos(10, 5);
	printf("Mission : 교도관과 터렛이 쏘는 총을 피해 탈출하라");

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	SetCurrentCursorPos(10, 7);
	printf("게임 설명 및 조작법");
	SetCurrentCursorPos(10, 9);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
	printf("방향키 좌우 (→,←) : 주인공인 송박이를 좌우로 움직임");
	SetCurrentCursorPos(10, 10);
	printf("방향키 상하 (↑,↓) : 사다리에서 송박이가 상하로 움직임");
	SetCurrentCursorPos(10, 11);
	printf("스페이스 바 (Space Bar) : 송박이의 비장의 무기 은신 ! ");

	SetCurrentCursorPos(10, 13);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf("아이템 설명");
	SetCurrentCursorPos(10, 15);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printf("◎");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf(" : 100점짜리 아이템         ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printf("◈");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf(" : 스위치(출구를 열어줌)");
	SetCurrentCursorPos(10, 16);
	printf("▩ : 닫혀있는 출구            □ : 열린 출구");
	SetCurrentCursorPos(10, 17);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	printf("★");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf(" : 순찰하는 교도관          ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	printf("→");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf(" : 터렛이 쏘는 총");

	SetCurrentCursorPos(50, 3);
	printf("PLAY GAME : SPACE BAR\n");

	while (1) {
		if (_kbhit() != 0) {
			int key = _getch();
			if (key == SPACE)
				break;
		}
	}
}
void SetGameBoard()
{
	int i = 0;
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
	{
		if (y == 1 || y == 7 || y == 13 || y == 19)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 0 && y != 0 || x == 39 && y != 0)
		{
			GamePlayBoard[y][x] = 1;
		}
		else
		{
			GamePlayBoard[y][x] = 0;
		}
	}

	// 사다리 위치 설정
	Ladder(30, 6);
	Ladder(15, 12);

	// 도착점 위치 설정
	GamePlayBoard[6][2] = 31;

	// 열쇠 위치 설정
	GamePlayBoard[12][12] = 24;

	// 동전 위치 설정
	GamePlayBoard[18][14] = 22;
	GamePlayBoard[18][34] = 22;
	GamePlayBoard[12][36] = 22;
	GamePlayBoard[12][8] = 22;
	// 터렛 시작 지역 설정
	GamePlayBoard[7][10] = 6;


	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
		Original_GamePlayBoard[y][x] = GamePlayBoard[y][x];

}
void SetGameBoard2()
{
	int i = 0;
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
	{
		if (y == 1 || y == 7 || y == 13 || y == 19)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 0 && y != 0 || x == 39 && y != 0)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 35 && y <= 13 && y >= 7)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 11 && y <= 20 && y >= 7)
		{
			GamePlayBoard[y][x] = 1;
		}

		else
		{
			GamePlayBoard[y][x] = 0;
		}
	}

	// 사다리 위치 설정
	Ladder(1, 6);
	Ladder(36, 6);
	Ladder(36, 12);
	Ladder(8, 12);
	Ladder(12, 12);

	// 은신 금지 구역 설정
	ProhibitHideArea(5, 7, 10, 7);
	ProhibitHideArea(14, 19, 17, 19);
	ProhibitHideArea(20, 19, 23, 19);
	ProhibitHideArea(26, 19, 29, 19);

	// 도착점 위치 설정
	GamePlayBoard[18][1] = 31;

	// 스위치 위치 설정
	GamePlayBoard[12][25] = 24;

	// 동전 위치 설정
	GamePlayBoard[6][25] = 22;
	GamePlayBoard[12][1] = 22;
	GamePlayBoard[12][32] = 22;
	GamePlayBoard[18][15] = 22;


	/// 터렛 위치 설정
	GamePlayBoard[13][2] = 6;
	GamePlayBoard[7][4] = 6;
	GamePlayBoard[7][32] = 6;



	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
		Original_GamePlayBoard[y][x] = GamePlayBoard[y][x];
}
void SetGameBoard3()
{
	int i = 0;
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
	{
		if (y == 1 || y == 7 || y == 13 || y == 19)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 0 && y != 0 || x == 39 && y != 0)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 27 && y <= 13 && y >= 0)
		{
			GamePlayBoard[y][x] = 1;
		}
		else if (x == 11 && y <= 20 && y >= 7)
		{
			GamePlayBoard[y][x] = 1;
		}

		else
		{
			GamePlayBoard[y][x] = 0;
		}
	}

	// 사다리 위치 설정
	Ladder(1, 6);
	Ladder(28, 12);
	Ladder(36, 6);
	Ladder(8, 12);
	Ladder(12, 12);
	Ladder(24, 6);

	// 은신 금지 구역 설정
	ProhibitHideArea(16, 7, 20, 7);
	ProhibitHideArea(1, 13, 7, 13);
	ProhibitHideArea(16, 13, 18, 13);
	ProhibitHideArea(21, 13, 23, 13);
	ProhibitHideArea(32, 13, 37, 13);
	ProhibitHideArea(13, 19, 15, 19);
	ProhibitHideArea(18, 19, 20, 19);

	// 도착점 위치 설정
	GamePlayBoard[18][37] = 31;

	// 스위치 위치 설정
	GamePlayBoard[6][30] = 24;

	// 동전 위치 설정
	GamePlayBoard[6][12] = 22;
	GamePlayBoard[12][24] = 22;
	GamePlayBoard[18][20] = 22;
	GamePlayBoard[18][30] = 22;

	// 터렛 위치 설정
	GamePlayBoard[19][36] = 6;
	GamePlayBoard[13][14] = 6;



	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
		Original_GamePlayBoard[y][x] = GamePlayBoard[y][x];
}
void SetInformation()
{
	COORD pp = pt;
	SetCurrentCursorPos(0, 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (int i = 0; i < Information.life; i++)
	{
		if (i == 0)
		{
			printf("LIFE :");
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		printf("♥");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	printf("\t\t\t Time : %d  \t\t\tScore : %d\n", Information.time, Information.score);
	SetCurrentCursorPos(pp.X, pp.Y);
}
void GameClear()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
	{
		SetCurrentCursorPos(x * 2, y);


		if (y == 1 || y == 19)
		{
			printf("■");
		}
		else if (x == 0 && y != 0 || x == 39 && y != 0)
		{
			printf("■");
		}
		else
		{
			printf("  ");
		}
	}
	SetCurrentCursorPos(36, 10);
	printf("< Game Clear >");
}
void GameOver()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	for (int y = 0; y < 20; y++)
	for (int x = 0; x < 40; x++)
	{
		SetCurrentCursorPos(x * 2, y);


		if (y == 1 || y == 19)
		{
			printf("■");
		}
		else if (x == 0 && y != 0 || x == 39 && y != 0)
		{
			printf("■");
		}
		else
		{
			printf("  ");
		}
	}
	SetCurrentCursorPos(34, 10);
	printf("< Game Over >");
	SetCurrentCursorPos(34, 12);
	printf("< Score : %d >", Information.score);

}
void DrawSetGameBoard(int GamePlayBoard[][40])
{

	for (int i = 1; i < 20; i++) {
		for (int j = 0; j < 40; j++) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			SetCurrentCursorPos(j * 2, i);
			if (GamePlayBoard[i][j] == -1)
				printf("●");
			else if (GamePlayBoard[i][j] == 1)
				printf("■");
			else if (GamePlayBoard[i][j] == 3) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				printf("─");
			}
			else if (GamePlayBoard[i][j] == 5) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
				printf("■");
			}
			else if (GamePlayBoard[i][j] == 6)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
				printf("■");
			}
			else if (GamePlayBoard[i][j] == 22) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				printf("◎");
			}
			else if (GamePlayBoard[i][j] == 24) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				printf("◈");
			}
			else if (GamePlayBoard[i][j] == 31)
				printf("▩");
			else if (GamePlayBoard[i][j] == 32)
				printf("□");

			else
				printf("  ");
		}
	}
	SetCurrentCursorPos(34, 2);
	printf("< %d STAGE >", current_stage_num);

	SetInformation();
}
void Ladder(int x, int y)
{
	GamePlayBoard[y][x] = 0;
	GamePlayBoard[y][x + 1] = 4;
	GamePlayBoard[y][x + 2] = 0;

	for (int i = 1; i <= 5; i++) {
		GamePlayBoard[y + i][x] = 2;
		GamePlayBoard[y + i][x + 1] = 3;
		GamePlayBoard[y + i][x + 2] = 2;
	}

	GamePlayBoard[y + 6][x] = 0;
	GamePlayBoard[y + 6][x + 1] = 4;
	GamePlayBoard[y + 6][x + 2] = 0;
}
void ProhibitHideArea(int x_s, int y_s, int x_d, int y_d)
{
	for (int i = x_s; i <= x_d; i++)
		GamePlayBoard[y_s][i] = 5;
}

void ProcessKeyInput()
{
	for (int i = 0; i < 10; i++) {

		if ((Check_Collition() == 1) || (Check_Collition() == 2))
			return;

		if (_kbhit() != 0) {
			int key = _getch();
			switch (key) {
			case LEFT: {
						   if (hide == 0)
							   ShiftLeft();
			}break;
			case RIGHT: {
							if (hide == 0)
								ShiftRight();
			}break;
			case UP: {
						 if (hide == 0)
							 ShiftUp();
			}break;
			case DOWN: {
						   if (hide == 0)
							   ShiftDown();
			}break;
			case SPACE: {
							if (DetectCollision(pt.X, pt.Y + 1) != 5)
								Hide();
			}break;
			default: break;
			}
		}
		Sleep(sleep_time);
		te++;
		if (hide == 1 && hide_time <= 0)
		{
			hide = 0;
			DrawCharacter();
		}

		if (te*sleep_time > 800)
		{

			Information.time -= 1;
			if (hide == 1)
			{
				hide_time -= 1;
				HideCharacter();
			}


			te = 0;
		}


		SetInformation();

	}
}

void DrawCharacter()
{
	COORD pos = pt;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	printf("●");
	SetCurrentCursorPos(pos.X, pos.Y);
}
void DeleteCharacter()
{
	COORD pos = pt;
	printf(" ");
	SetCurrentCursorPos(pos.X, pos.Y);
}
void HideCharacter()
{

	COORD pos = pt;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	if (hide_time == 1)
	{
		printf("①");
	}
	else if (hide_time == 2)
	{
		printf("②");
	}
	else if (hide_time == 3)
	{
		printf("③");
	}
	else
		printf("○");
	SetCurrentCursorPos(pos.X, pos.Y);
}

int ShiftRight()
{
	if (DetectCollision(pt.X + 2, pt.Y) == 1)
		return 0;
	if (DetectCollision(pt.X, pt.Y) == 3)
		return 0;

	DeleteCharacter();
	if (GamePlayBoard[pt.Y][pt.X / 2] == 31) {
		COORD pp = pt;
		SetCurrentCursorPos(pp.X, pp.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("▩");
		SetCurrentCursorPos(pt.X, pt.Y);
	}
	pt.X += 2;
	client_cur_pos.X += 2;
	SetCurrentCursorPos(pt.X, pt.Y);
	DrawCharacter();

	return 1;
}
int ShiftLeft()
{
	if (DetectCollision(pt.X - 2, pt.Y) == 1)
		return 0;
	if (DetectCollision(pt.X, pt.Y) == 3)
		return 0;

	DeleteCharacter();
	if (GamePlayBoard[pt.Y][pt.X / 2] == 31) {
		COORD pp = pt;
		SetCurrentCursorPos(pp.X, pp.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("▩");
		SetCurrentCursorPos(pt.X, pt.Y);
	}
	pt.X -= 2;
	SetCurrentCursorPos(pt.X, pt.Y);
	client_cur_pos.X -= 2;
	DrawCharacter();
	return 1;
}
int ShiftUp()
{
	if (DetectCollision(pt.X, pt.Y - 1) == 1)
		return 0;

	if (DetectCollision(pt.X, pt.Y - 1) == 3) {

		if (DetectCollision(pt.X, pt.Y) != 4) {
			COORD pp = pt;
			SetCurrentCursorPos(pp.X, pp.Y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			printf("─");
		}

		DeleteCharacter();
		pt.Y--;
		client_cur_pos.Y -= 1;
		SetCurrentCursorPos(pt.X, pt.Y);
		DrawCharacter();
		return 0;
	}
	else if (DetectCollision(pt.X, pt.Y - 1) == 4) {
		COORD pp = pt;
		SetCurrentCursorPos(pp.X, pp.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		printf("─");

		DeleteCharacter();
		pt.Y--;
		client_cur_pos.Y -= 1;
		SetCurrentCursorPos(pt.X, pt.Y);
		DrawCharacter();
		return 0;
	}
	return 1;
}
int ShiftDown()
{
	if (DetectCollision(pt.X, pt.Y + 1) == 1)
		return 0;

	if (DetectCollision(pt.X, pt.Y + 1) == 3) {
		if (DetectCollision(pt.X, pt.Y) != 4) {
			COORD pp = pt;
			SetCurrentCursorPos(pp.X, pp.Y);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
			printf("─");
		}

		DeleteCharacter();
		pt.Y++;
		client_cur_pos.Y += 1;
		SetCurrentCursorPos(pt.X, pt.Y);
		DrawCharacter();
		return 0;
	}
	else if (DetectCollision(pt.X, pt.Y + 1) == 4) {
		COORD pp = pt;
		SetCurrentCursorPos(pp.X, pp.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
		printf("─");

		DeleteCharacter();
		pt.Y++;
		client_cur_pos.Y += 1;
		SetCurrentCursorPos(pt.X, pt.Y);
		DrawCharacter();
		return 0;
	}
	return 1;
}
int Jump()
{
	for (int i = 0; i < 3; i++) {
		DeleteCharacter();
		//GamePlayBoard[pt.Y][pt.X / 2] = 0;
		pt.Y--;
		//GamePlayBoard[pt.Y][pt.X / 2] = -1;
		SetCurrentCursorPos(pt.X, pt.Y);
		DrawCharacter();
		for (int i = 0; i < 2; i++) {
			if (_kbhit() != 0) {
				int key = _getch();
				switch (key) {
				case LEFT: ShiftLeft(); break;
				case RIGHT: ShiftRight(); break;
				case UP: {
							 ShiftUp();
				}break;
				case DOWN: {
							   ShiftDown();
				}break;
				default: break;
				}
			}
			Sleep(20);
		}
		Sleep(100);
	}
	Sleep(50);


	for (int i = 0; i < 3; i++) {
		DeleteCharacter();
		//GamePlayBoard[pt.Y][pt.X / 2] = 0;
		pt.Y++;
		//GamePlayBoard[pt.Y][pt.X / 2] = -1;
		SetCurrentCursorPos(pt.X, pt.Y);
		DrawCharacter();
		for (int i = 0; i < 2; i++) {
			if (_kbhit() != 0) {
				int key = _getch();
				switch (key) {
				case LEFT: ShiftLeft(); break;
				case RIGHT: ShiftRight(); break;
				case UP: {
							 ShiftUp();
				}break;
				case DOWN: {
							   ShiftDown();
				}break;
				default: break;
				}
			}
			Sleep(20);
		}
		Sleep(100);
	}


	return 1;
}
void Hide() {
	if (hide == 0) {
		HideCharacter();
		hide = 1;
		hide_time = 4;
	}
	else if (hide == 1) {
		DrawCharacter();
		hide_time = 0;
		hide = 0;
	}
}

void set_npc()
{
	int i, j;
	///////////////////


	my_npc = (struct npc **)malloc(sizeof(struct npc *) *total_stage_num); //npc 이차원배열로 선언 [i][j] = i번째 스테이지의 j번째 npc
	for (i = 0; i < total_stage_num; i++)
	{
		my_npc[i] = (struct npc*)malloc(sizeof(struct npc)*total_npc_count);
	}

	///////////////// //초기 npc설정
	my_npc[1][0].npc_start_x = 26 * 2;
	my_npc[1][0].npc_start_y = 12;
	my_npc[1][0].npc_destination_x = 26 * 2;
	my_npc[1][0].npc_destination_y = 18;
	my_npc[1][0].vector = -2;

	my_npc[1][1].npc_start_x = 10 * 2;
	my_npc[1][1].npc_start_y = 18;
	my_npc[1][1].npc_destination_x = 20 * 2;
	my_npc[1][1].npc_destination_y = 18;
	my_npc[1][1].vector = 2;

	my_npc[1][2].npc_start_x = 20 * 2;
	my_npc[1][2].npc_start_y = 6;
	my_npc[1][2].npc_destination_x = 20 * 2;
	my_npc[1][2].npc_destination_y = 12;
	my_npc[1][2].vector = 2;

	/////////////////////////////////

	my_npc[2][0].npc_start_x = 10 * 2;
	my_npc[2][0].npc_start_y = 6;
	my_npc[2][0].npc_destination_x = 30 * 2;
	my_npc[2][0].npc_destination_y = 6;
	my_npc[2][0].vector = 2;

	my_npc[2][1].npc_start_x = 16 * 2;
	my_npc[2][1].npc_start_y = 18;
	my_npc[2][1].npc_destination_x = 38 * 2;
	my_npc[2][1].npc_destination_y = 18;
	my_npc[2][1].vector = 2;

	my_npc[2][2].npc_start_x = 20 * 2;
	my_npc[2][2].npc_start_y = 12;
	my_npc[2][2].npc_destination_x = 20 * 2;
	my_npc[2][2].npc_destination_y = 18;
	my_npc[2][2].vector = -2;

	my_npc[2][3].npc_start_x = 30 * 2;
	my_npc[2][3].npc_start_y = 18;
	my_npc[2][3].npc_destination_x = 38 * 2;
	my_npc[2][3].npc_destination_y = 12;
	my_npc[2][3].vector = 2;

	//////////////////////////////////

	my_npc[3][0].npc_start_x = 12 * 2;
	my_npc[3][0].npc_start_y = 6;
	my_npc[3][0].npc_destination_x = 4 * 2;
	my_npc[3][0].npc_destination_y = 12;
	my_npc[3][0].vector = -2;

	my_npc[3][1].npc_start_x = 14 * 2;
	my_npc[3][1].npc_start_y = 6;
	my_npc[3][1].npc_destination_x = 14 * 2;
	my_npc[3][1].npc_destination_y = 12;
	my_npc[3][1].vector = 2;

	my_npc[3][2].npc_start_x = 8 * 2;
	my_npc[3][2].npc_start_y = 6;
	my_npc[3][2].npc_destination_x = 20 * 2;
	my_npc[3][2].npc_destination_y = 6;
	my_npc[3][2].vector = -2;

	my_npc[3][3].npc_start_x = 20 * 2;
	my_npc[3][3].npc_start_y = 18;
	my_npc[3][3].npc_destination_x = 20 * 2;
	my_npc[3][3].npc_destination_y = 12;
	my_npc[3][3].vector = -2;

	my_npc[3][4].npc_start_x = 18 * 2;
	my_npc[3][4].npc_start_y = 18;
	my_npc[3][4].npc_destination_x = 39 * 2; //
	my_npc[3][4].npc_destination_y = 18;
	my_npc[3][4].vector = -2;

	my_npc[3][5].npc_start_x = 34 * 2;
	my_npc[3][5].npc_start_y = 12;
	my_npc[3][5].npc_destination_x = 34 * 2;
	my_npc[3][5].npc_destination_y = 18;
	my_npc[3][5].vector = -2;

	my_npc[3][6].npc_start_x = 32 * 2;
	my_npc[3][6].npc_start_y = 12;
	my_npc[3][6].npc_destination_x = 32 * 2;
	my_npc[3][6].npc_destination_y = 6;
	my_npc[3][6].vector = 2;

	my_npc[3][7].npc_start_x = 29 * 2;
	my_npc[3][7].npc_start_y = 6;
	my_npc[3][7].npc_destination_x = 29 * 2;
	my_npc[3][7].npc_destination_y = 12;
	my_npc[3][7].vector = 2;



	//////////////////


	////npc 시작 위치 정의//

	for (i = 0; i < total_stage_num; i++)
	{
		for (j = 0; j < total_npc_count; j++)
		{
			my_npc[i][j].current_pos.X = my_npc[i][j].npc_start_x;
			my_npc[i][j].current_pos.Y = my_npc[i][j].npc_start_y;


			my_npc[i][j].npc_temp_pos.X = my_npc[i][j].npc_destination_x;
			my_npc[i][j].npc_temp_pos.Y = my_npc[i][j].npc_destination_y;
			my_npc[i][j].original_start.X = my_npc[i][j].npc_start_x;
			my_npc[i][j].original_start.Y = my_npc[i][j].npc_start_y;
			my_npc[i][j].original_destination.X = my_npc[i][j].npc_destination_x;
			my_npc[i][j].original_destination.Y = my_npc[i][j].npc_destination_y;
			my_npc[i][j].flag_count = 0;

			my_npc[i][j].downflag = 0;
			my_npc[i][j].upflag = 0;

		}
	}
}


////////////npc이동시 다시 그려주는 함수////////////////////


void redraw(int i)
{

	SetCurrentCursorPos(my_npc[current_stage_num][i].current_pos.X, my_npc[current_stage_num][i].current_pos.Y);


	if (my_npc[current_stage_num][i].current_pos.X == client_cur_pos.X && my_npc[current_stage_num][i].current_pos.Y == client_cur_pos.Y)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		HideCharacter();
	}
	else if (item_is_taken == FALSE && Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 22)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		printf("◎");
	}
	else if (Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 31)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("▩");
	}

	else if (Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 32)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("□");
	}
	else if (Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 3)
	{
		printf("ㅡ");
	}
	else
	{
		printf(" ");
	}
	//if (Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][(my_npc[current_stage_num][i].current_pos.X / 2)] != 22 && )
	GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][(my_npc[current_stage_num][i].current_pos.X / 2)] = Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][(my_npc[current_stage_num][i].current_pos.X / 2)];//게임보드 좌표를 원래 값으로 변경




}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////터렛 이동시 다시 그려주는 함수////////////////////

void redraw_turret(int i)
{

	SetCurrentCursorPos(my_turret[current_stage_num][i].current_pos.X, my_turret[current_stage_num][i].current_pos.Y);


	if (my_turret[current_stage_num][i].current_pos.X == client_cur_pos.X && my_turret[current_stage_num][i].current_pos.Y == client_cur_pos.Y)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		HideCharacter();
	}
	else if (item_is_taken == FALSE && Original_GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][my_turret[current_stage_num][i].current_pos.X / 2] == 22)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		printf("◎");
	}
	else if (Original_GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][my_turret[current_stage_num][i].current_pos.X / 2] == 31)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("▩");
	}
	else if (Original_GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][my_turret[current_stage_num][i].current_pos.X / 2] == 32)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		printf("□");
	}
	else if (Original_GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][my_turret[current_stage_num][i].current_pos.X / 2] == 3)
	{
		printf("ㅡ");
	}
	else
	{
		printf(" ");
	}

	GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][(my_turret[current_stage_num][i].current_pos.X / 2)] = Original_GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][(my_turret[current_stage_num][i].current_pos.X / 2)];//게임보드 좌표를 원래 값으로 변경




}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int move_npc()
{

	COORD temp = GetCurrentCursorPos();
	int i;


	for (i = 0; i < current_npc_count; i++) //현재 npc갯수까지 움직임 제어
	{

		redraw(i);


		//SetCurrentCursorPos(my_npc[current_stage_num][i].npc_destination_x, my_npc[current_stage_num][i].npc_destination_y);
		//printf("※");
		/////////////////////////////////////////////////////////추적 npc 설정///////////////////////////////////////

		double distance = sqrt((my_npc[current_stage_num][i].current_pos.X / 2 - pt.X / 2)*(my_npc[current_stage_num][i].current_pos.X / 2 - pt.X / 2) + (my_npc[current_stage_num][i].current_pos.Y - pt.Y) *(my_npc[current_stage_num][i].current_pos.Y - pt.Y));
		////////////////////// npc 행동 예외처리 /////////

		if (my_npc[current_stage_num][i].current_pos.X == my_npc[current_stage_num][i].npc_destination_x && my_npc[current_stage_num][i].current_pos.Y == my_npc[current_stage_num][i].npc_destination_y) // 도착시 시작과 끝 바꾸고 벡터 방향 바꾸기
		{
			COORD temp;
			temp.X = my_npc[current_stage_num][i].npc_start_x;
			my_npc[current_stage_num][i].npc_start_x = my_npc[current_stage_num][i].npc_destination_x;
			my_npc[current_stage_num][i].npc_destination_x = temp.X;

			temp.Y = my_npc[current_stage_num][i].npc_start_y;
			my_npc[current_stage_num][i].npc_start_y = my_npc[current_stage_num][i].npc_destination_y;
			my_npc[current_stage_num][i].npc_destination_y = temp.Y;


			my_npc[current_stage_num][i].npc_temp_pos.X = my_npc[current_stage_num][i].npc_destination_x;
			my_npc[current_stage_num][i].npc_temp_pos.Y = my_npc[current_stage_num][i].npc_destination_y;

			my_npc[current_stage_num][i].vector *= -1;
			//}




		}
		// 위로 올라가는 부분
		else if (Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 4 && Original_GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y - 1][my_npc[current_stage_num][i].current_pos.X / 2] == 3 && my_npc[current_stage_num][i].current_pos.Y> my_npc[current_stage_num][i].npc_destination_y)// 사다리 아래에서 위로 올라가는 경우 정의
		{
			my_npc[current_stage_num][i].upflag = 1;



		} // 아래로 내려가는 부분

		else if (GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 4 && GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y + 1][my_npc[current_stage_num][i].current_pos.X / 2] == 3 && my_npc[current_stage_num][i].current_pos.Y < my_npc[current_stage_num][i].npc_destination_y)
		{
			my_npc[current_stage_num][i].downflag = 1;
		}
		else if (GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2 + my_npc[current_stage_num][i].vector] == 1)
		{
			//if (GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] != 3)
			my_npc[current_stage_num][i].vector *= -1;

		}


		if ((my_npc[current_stage_num][i].upflag == 1) && my_npc[current_stage_num][i].flag_count == 6)
		{
			my_npc[current_stage_num][i].flag_count = 0;
			my_npc[current_stage_num][i].upflag = 0;
			my_npc[current_stage_num][i].downflag = 0;

			if (my_npc[current_stage_num][i].npc_start_x < my_npc[current_stage_num][i].npc_destination_x)
				my_npc[current_stage_num][i].vector = 2;
			else if (my_npc[current_stage_num][i].npc_start_x > my_npc[current_stage_num][i].npc_destination_x)
			{
				my_npc[current_stage_num][i].vector = -2;
			}


		}
		else if ((my_npc[current_stage_num][i].downflag == 1) && my_npc[current_stage_num][i].flag_count == 6)
		{
			my_npc[current_stage_num][i].flag_count = 0;
			my_npc[current_stage_num][i].downflag = 0;
			my_npc[current_stage_num][i].upflag = 0;
			if (my_npc[current_stage_num][i].npc_start_x < my_npc[current_stage_num][i].npc_destination_x)
				my_npc[current_stage_num][i].vector = 2;
			else if (my_npc[current_stage_num][i].npc_start_x > my_npc[current_stage_num][i].npc_destination_x)
			{
				my_npc[current_stage_num][i].vector = -2;
			}

		}




		////////////////////////움직임 정의/////////////////////
		if (my_npc[current_stage_num][i].upflag == 1)
		{
			SetCurrentCursorPos(my_npc[current_stage_num][i].current_pos.X, my_npc[current_stage_num][i].current_pos.Y);
			if ((my_npc[current_stage_num][i].upflag == 1 || my_npc[current_stage_num][i].downflag == 1) && (GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 3)) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				redraw(i);
			}
			my_npc[current_stage_num][i].current_pos.Y += -1;
			my_npc[current_stage_num][i].flag_count++;
			SetCurrentCursorPos(my_npc[current_stage_num][i].current_pos.X, my_npc[current_stage_num][i].current_pos.Y);
			GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][(my_npc[current_stage_num][i].current_pos.X / 2)] = 15; //게임보드 좌표 변경
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			printf("★");
		}
		else if (my_npc[current_stage_num][i].downflag == 1)
		{
			SetCurrentCursorPos(my_npc[current_stage_num][i].current_pos.X, my_npc[current_stage_num][i].current_pos.Y);
			if ((my_npc[current_stage_num][i].upflag == 1 || my_npc[current_stage_num][i].downflag == 1) && (GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][my_npc[current_stage_num][i].current_pos.X / 2] == 3)) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
				redraw(i);
			}
			my_npc[current_stage_num][i].flag_count++;
			my_npc[current_stage_num][i].current_pos.Y += 1;

			SetCurrentCursorPos(my_npc[current_stage_num][i].current_pos.X, my_npc[current_stage_num][i].current_pos.Y);
			GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][(my_npc[current_stage_num][i].current_pos.X / 2)] = 15; //게임보드 좌표 변경
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			printf("★");
		}
		else if (my_npc[current_stage_num][i].downflag == 0 && my_npc[current_stage_num][i].upflag == 0)
		{
			my_npc[current_stage_num][i].current_pos.X += my_npc[current_stage_num][i].vector;
			SetCurrentCursorPos(my_npc[current_stage_num][i].current_pos.X, my_npc[current_stage_num][i].current_pos.Y);
			GamePlayBoard[my_npc[current_stage_num][i].current_pos.Y][(my_npc[current_stage_num][i].current_pos.X / 2)] = 15; //게임보드 좌표 변경
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			printf("★");
		}


	}

	SetCurrentCursorPos(temp.X, temp.Y);



	return 0;
}
void set_turret()
{
	int i, j, k;

	my_turret = (struct turret **)malloc(sizeof(struct turret*)* total_stage_num);

	for (i = 0; i < total_stage_num; i++)
	{
		my_turret[i] = (struct turret *)malloc(sizeof(struct turret)* total_turret_count);
	}
	///초기 터렛 설정

	my_turret[1][0].start_x = 10 * 2;
	my_turret[1][0].pos_y = 6;
	my_turret[1][0].vector = 2;

	my_turret[1][0].current_pos.X = my_turret[1][0].start_x;
	my_turret[1][0].current_pos.Y = my_turret[1][0].pos_y;

	//////////////////////////////////////////////////////////
	my_turret[2][0].start_x = 1 * 2;
	my_turret[2][0].pos_y = 12;
	my_turret[2][0].vector = 2;

	my_turret[2][0].current_pos.X = my_turret[2][0].start_x;
	my_turret[2][0].current_pos.Y = my_turret[2][0].pos_y;

	my_turret[2][1].start_x = 4 * 2;
	my_turret[2][1].pos_y = 6;
	my_turret[2][1].vector = 2;

	my_turret[2][1].current_pos.X = my_turret[2][1].start_x;
	my_turret[2][1].current_pos.Y = my_turret[2][1].pos_y;

	my_turret[2][2].start_x = 32 * 2;
	my_turret[2][2].pos_y = 6;
	my_turret[2][2].vector = -2;

	my_turret[2][2].current_pos.X = my_turret[2][2].start_x;
	my_turret[2][2].current_pos.Y = my_turret[2][2].pos_y;


	//////////////////////////////////////////////////////////

	my_turret[3][0].start_x = 36 * 2;
	my_turret[3][0].pos_y = 18;
	my_turret[3][0].finish_x = 5 * 2;
	my_turret[3][0].vector = -2;

	my_turret[3][0].current_pos.X = my_turret[3][0].start_x;
	my_turret[3][0].current_pos.Y = my_turret[3][0].pos_y;

	my_turret[3][1].start_x = 14 * 2;
	my_turret[3][1].pos_y = 12;
	my_turret[3][1].finish_x = 5 * 2;
	my_turret[3][1].vector = 2;


	my_turret[3][1].current_pos.X = my_turret[3][1].start_x;
	my_turret[3][1].current_pos.Y = my_turret[3][1].pos_y;
}
void move_turret()
{
	int i, j, k;
	COORD temp2 = GetCurrentCursorPos();

	for (i = 0; i< current_turret_count; i++)
	{
		SetCurrentCursorPos(my_turret[current_stage_num][i].current_pos.X, my_turret[current_stage_num][i].current_pos.Y);
		redraw_turret(i);

		if (GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][my_turret[current_stage_num][i].current_pos.X / 2 + (my_turret[current_stage_num][i].vector)] == 1)
			my_turret[current_stage_num][i].current_pos.X = my_turret[current_stage_num][i].start_x;



		my_turret[current_stage_num][i].current_pos.X += my_turret[current_stage_num][i].vector;
		GamePlayBoard[my_turret[current_stage_num][i].current_pos.Y][my_turret[current_stage_num][i].current_pos.X / 2] = 15;
		SetCurrentCursorPos(my_turret[current_stage_num][i].current_pos.X, my_turret[current_stage_num][i].current_pos.Y);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		if (my_turret[current_stage_num][i].vector > 0)
			printf("→");
		else
			printf("←");

		SetCurrentCursorPos(temp2.X, temp2.Y);

	}
}


int DetectCollision(int posX, int posY)
{
	int arrX = posX / 2;
	int arrY = posY;
	if (GamePlayBoard[arrY][arrX] == 1)         // 벽 충돌
		return 1;
	if (GamePlayBoard[arrY][arrX] == 3)         // 사다리(종) 충돌
		return 3;
	if (GamePlayBoard[arrY][arrX] == 4)         // 사다리 도입부
		return 4;
	if (GamePlayBoard[arrY][arrX] == 5)         // 은신 금지 구역
		return 5;
	if (GamePlayBoard[arrY][arrX] == 15)      // 교도관 충돌
		return 15;
	if (GamePlayBoard[arrY][arrX] == 22)      // 동전(아이템)
		return 22;
	if (GamePlayBoard[arrY][arrX] == 24)      // 열쇠(아이템)
		return 24;
	if (GamePlayBoard[arrY][arrX] == 32)      // 열린 도착점
		return 32;

	return 0;
}
int Check_Collition()
{
	if (hide == 1)
		return 0;

	int check;

	check = DetectCollision(pt.X, pt.Y);
	switch (check) {

	case 15: return 1;      // 교도관 충돌
	case 16: return 1;      // 총알 충돌
	case 22: {            // 아이템 동전 습득
				 item_Coin();

	}break;
	case 24: {            // 아이템 스위치 습득
				 item_Switch();
	}break;
	case 32: return 2;      // 출입문 도착

	default: break;
	}

	return 0;
}
void item_Switch()
{
	GamePlayBoard[pt.Y][pt.X / 2] = 0;
	for (int i = 0; i < 20; i++)
	for (int j = 0; j < 40; j++)
	if (GamePlayBoard[i][j] == 31) {
		GamePlayBoard[i][j] = 32;
		sleep_time = 15;
		COORD pp = pt;
		SetCurrentCursorPos(j * 2, i);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		// 
		key_is_taken = TRUE;
		printf("□");
		SetCurrentCursorPos(pp.X, pp.Y);

		PlaySound(TEXT("loop3.wav"), NULL, SND_ASYNC);
		Sleep(500);
		PlaySound(TEXT("loop2.wav"), NULL, SND_ASYNC | SND_LOOP);

	}
}
void item_Coin()
{
	Information.score += 100;
	GamePlayBoard[pt.Y][pt.X / 2] = 0;
	Original_GamePlayBoard[pt.Y][pt.X / 2] = 0;
	SetInformation();
}

void stage3_Event()
{
	if (((pt.X == (34 * 2)) || (pt.X == (33 * 2))) && (pt.Y == 6) && (current_stage_num == 3) && (flag == 0)) {
		current_npc_count = 8;
		flag = 1;
		GamePlayBoard[18][2] = 24;
		GamePlayBoard[6][30] = 0;
		COORD pp = pt;
		SetCurrentCursorPos(2 * 2, 18);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		printf("◈");
		SetCurrentCursorPos(pp.X, pp.Y);
	}
}

int main()
{
	RemoveCursor();
	initInformation();
	set_npc();
	set_turret();

	while (1) {
		PlaySound(TEXT("loop2.wav"), NULL, SND_ASYNC | SND_LOOP);
		if (current_stage_num == 0) {            // 메뉴얼 보여주기
			manualBoard();
			current_stage_num++;
		}

		if (Information.life == 0) {            // Life가 '0'이 되었을때
			GameOver();
			break;
		}
		if (current_stage_num == 4) {            // 스테이지 모두 클리어
			GameClear();
			break;
		}

		SetCurrentCursorPos(0, 0);
		if (current_stage_num == 1)
		{

			Information.time = 40;
			SetGameBoard();
		}
		else if (current_stage_num == 2)
		{
			Information.time = 120;
			SetGameBoard2();
		}
		else if (current_stage_num == 3) {
			Information.time = 200;
			set_npc();
			SetGameBoard3();
			current_npc_count = 7;
			flag = 0;
		}

		Information.score = Information.original_score;
		DrawSetGameBoard(Original_GamePlayBoard);
		//SetCurrentCursorPos(4 * 2, 18);         // PC 시작위치
		SetCurrentCursorPos(4 * 2, 18);
		sleep_time = 20;

		///////////////////////pc 현재 위치 정의//////////////////////////////
		//client_cur_pos.X = 4 * 2;
		//client_cur_pos.Y = 18;
		client_cur_pos.X = 4 * 2;
		client_cur_pos.Y = 18;
		key_is_taken = FALSE;
		item_is_taken = FALSE;
		//////////////////////////////////////////////////////

		DrawCharacter();

		while (1) {
			if (Check_Collition() == 1 || Information.time <= 0) {      // PC 사망의 경우

				Information.life--;
				Information.score = Information.original_score;
				break;
			}
			if (Check_Collition() == 2) {      // 스테이지 클리어의 경우
				current_stage_num++;
				Information.original_score = Information.score + Information.life * 100;
				Information.life = 3;
				if (current_stage_num == 2) {
					current_npc_count = 4;
					current_turret_count = 3;
				}
				else if (current_stage_num == 3) {
					current_npc_count = 7;
					current_turret_count = 2;
				}

				break;
			}
			stage3_Event();
			move_npc();
			move_turret();
			ProcessKeyInput();
		}
	}

	SetCurrentCursorPos(0, 21);

	return 0;
}