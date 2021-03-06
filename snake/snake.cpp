// snake.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "output.h"
#include <conio.h>
#include <ctime>
#include <math.h>

#define CEIL 0
#define FLOOR 29
#define LEFT_WALL 0
#define RIGHT_WALL 29
#define SPEED_FACTOR 300000
#define SCORE_FACTOR 100000000

int HeadDirection[4][2] = { // Snake의 이동 방향을 연산하는데 사용될 배열
	{0,-1},//up
	{0,1},//down
	{-1,0},//left
	{1,0}//right
};
int CurrentHeadDirection = 0; // HeadDirection의 4개의 Condition 중 현재의 Condition
int GatheredFood = 0; // 채집한 먹이 수
int GameScore = 0; // 게임 점수
int SnakeSpeed = 0; // 뱀의 속도. clock()함수의 인자값을 고려
int BombCount = 0; // 터진 먹이 폭탄의 수
int RemainCount = 0; // 남은 먹이 폭탄의 잔해 수
int pivot = 1;
int PassedTime = 0;
bool IsFeverTime = false;
double FeverFactor = 1;


void printMap() // Debug용 Map 배열 출력 함수
{
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			printf("%d ", MapCheckArray[i][j]);
		}
		printf("\n");
	}
}

bool checkMap() // 게임이 끝났을 때, 플레이어의 승리 여부를 판단하기 위한 Map 배열 체크 함수
{
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (MapCheckArray[i][j] == 0)return false;
		}
	}
	return true;
}

struct Node // 뱀의 몸통 분자들의 좌표 정보를 담는 구조체
{
	int x;
	int y;
	void setX(int cx) { x = cx; }
	void setY(int cy) { y = cy; }
	int getX() { return x; }
	int getY() { return y; }
}node;

struct BodyArrayList // 뱀의 몸통 분자들의 구조체를 담는 ArrayList꼴의 구조체
{
	Node *BodySet;
	int length = 3;
}BodyList;

void addBodyPart(Node part) // ArrayList에 인자 추가
{
	BodyList.BodySet[BodyList.length] = part;
	BodyList.length += 1;
}

void moveBody(int cx, int cy); // 몸을 앞의 분자의 뒤를 따라 움직이도록 하는 함수
void printDot(Node node); // 주어진 좌표에 점을 찍는 함수
void printSpace(Node node); // 주어진 좌표에 찍힌 점을 지우는 함수
void createFood(); // 맵에 음식을 생성하는 함수
void createCheatFood(); // Debug용, 치트키를 이용해 음식을 생성하는 함수


void printDot(Node node) // 주어진 좌표에 점을 찍는 함수
{
	//▲▼◀▶
	setCursurPos(node.getX() * 2, node.getY());
	switch (CurrentHeadDirection) { // 방향에 따른 점의 모양을 다르게 함
	case 0:
		printf("▲");
		break;
	case 1:
		printf("▼");
		break;
	case 2:
		printf("◀");
		break;
	case 3:
		printf("▶");
		break;
	default:
		break;
	}
}
//void printDot(Node node)
//{
//	setCursurPos(node.getX() * 2, node.getY());
//	printf("■");
//}
void printSpace(Node node) // 주어진 좌표에 찍힌 점을 지우는 함수
{
	setCursurPos(node.getX() * 2, node.getY());
	printf("  ");
}
void printSpace(int x, int y) // 주어진 좌표에 찍힌 점을 지우는 함수
{
	setCursurPos(x * 2, y);
	printf("  ");
}

int moveHead() // 머리를 움직이는 함수
{	
	bool isEat = false; // 먹이를 먹었는지 파악하는 불린형 변수

	int CurrentHeadPosX = BodyList.BodySet[0].getX(); // 현재 머리의 x좌표
	int CurrentHeadPosY = BodyList.BodySet[0].getY(); // 현재 머리의 y좌표

	int CurrentRearPosX = BodyList.BodySet[BodyList.length - 1].getX(); // 현재 꼬리의 x좌표
	int CurrentRearPosY = BodyList.BodySet[BodyList.length - 1].getY(); // 현재 꼬리의 y좌표

	int NextPositionX = BodyList.BodySet[0].getX() + HeadDirection[CurrentHeadDirection][0]; // 머리가 바로 다음에 진행 할 지점의 좌표
	int NextPositionY = BodyList.BodySet[0].getY() + HeadDirection[CurrentHeadDirection][1]; 

	if (MapCheckArray[NextPositionX][NextPositionY] == 1) { // 머리가 도달한 지점의 체크 배열 값이 1일 때
		
		if (checkMap() == true)return 3; // 모든 맵의 체크값이 1일 때 -> 플레이어의 승리
		return 0; // 하나 이상의 0이 있을 때 -> 플레이어의 패배
	}
	if (MapCheckArray[NextPositionX][NextPositionY] == 2) { // 도달한 지점에 음식이 있을 때
		isEat = true; // check the flag
	}

	BodyList.BodySet[0].setX(NextPositionX); // 머리 노드를 다음 장소로 이동
	BodyList.BodySet[0].setY(NextPositionY);

	MapCheckArray[BodyList.BodySet[0].x][BodyList.BodySet[0].y] = 1; // check Map array

	moveBody(CurrentHeadPosX, CurrentHeadPosY); // 몸의 다른 부분 이동

	if (isEat == true) { // 뱀이 음식을 먹었을 때
		Node NewRear; // 뱀의 꼬리 부분에 붙일 새로운 부분을 생성
		NewRear.setX(CurrentRearPosX); // set rear's x,y coordinate
		NewRear.setY(CurrentRearPosY);
		addBodyPart(NewRear);

		if (RemainCount > 0) { // 폭탄이 터졌고, 음식 잔해가 여전히 남아 있을 때
			RemainCount--; // 잔해 개수만 줄이고 새로운 음식은 생성하지 않는다
			setCursurPos(70, 21);
			printf("%d", RemainCount);
		}
		else if(RemainCount == 0) { // 폭탄의 잔해가 없을 때
			createFood(); // 새로운 음식 생성
			if (IsFeverTime == true) {
				createFood();
				RemainCount = 1;
			}
			setCursurPos(95, 21);
			printf("               ");
			int bombAmountDecider = (rand() % 8) + 2;
			int bombDecider = (rand() % 8) + 1;
			
			if (bombDecider >= 8) { // one in eight times, bomb explodes
				BombCount++;
				setCursurPos(70, 18);
				printf("%d", BombCount);
				RemainCount = bombAmountDecider;

				for (int i = 0; i < bombAmountDecider; i++) {
					createFood();
				}
				setCursurPos(95, 21);
				printf("Bomb Exploded!");

			}
			setCursurPos(70, 21);
			printf("%d", RemainCount);
		}

		isEat = false; // reset flag
		GameScore += 1000;//(int)((SCORE_FACTOR/SnakeSpeed)*FeverFactor); // renew the score
		GatheredFood++;
		setCursurPos(70, 15);
		printf("%d", GameScore);
		setCursurPos(70, 24);
		printf("%3d", GatheredFood);
		SnakeSpeed = (SPEED_FACTOR - GatheredFood*2000)/FeverFactor; // renew the speed
	}
	else if(isEat==false){
		printSpace(CurrentRearPosX,CurrentRearPosY); // 뱀의 잔상 제거
		MapCheckArray[CurrentRearPosX][CurrentRearPosY] = 0;
	}

	printDot(BodyList.BodySet[0]); // mark head and rear on the map
	printDot(BodyList.BodySet[BodyList.length-1]);

	setCursurPos(70, 5);
	printf("%d", BodyList.length);

	setCursurPos(70, 8);
	printf("%d", SnakeSpeed);

	return 1;
}
void moveBody(int cx, int cy) // function for moving other parts of body
{
	for (int i = BodyList.length - 1; i > 1; --i) { // each body moves forward
		BodyList.BodySet[i] = BodyList.BodySet[i - 1];
	}
	BodyList.BodySet[1].setX(cx);
	BodyList.BodySet[1].setY(cy);
}
int setHeadDirection(int key) // set direction where head will head
{
	switch (key) {
	case KEY_UP:
		if (CurrentHeadDirection != 1) { // 새로 들어온 명령의 방향이 현재 진행 방향과 정반대일 때,
			CurrentHeadDirection = 0; // 새로운 명령을 무시
		}
		break;
	case KEY_DOWN:
		if (CurrentHeadDirection != 0) {
			CurrentHeadDirection = 1;
		}
		break;
	case KEY_LEFT:
		if (CurrentHeadDirection != 3) {
			CurrentHeadDirection = 2;
		}
		break;
	case KEY_RIGHT:
		if (CurrentHeadDirection != 2) {
			CurrentHeadDirection = 3;
		}
		break;
	case 49:
		createFood(); // for debug, cheat key for placing more foods
		break;
	default:
		break;
	}
	return 0;
}

void createFood() // function for placeing a new food on map
{
	srand(time(NULL));
	while (1) {
		int ranX = rand() % (HEIGHT-2) + 1;
		int ranY = rand() % (WIDTH-2) + 1;
		if (MapCheckArray[ranX][ranY] == 0) {
			MapCheckArray[ranX][ranY] = 2;
			setCursurPos(ranX*2, ranY);
			printf("♥");
			setCursurPos(95, 4);
			printf("먹이 좌표");
			setCursurPos(95, 4+(pivot)%10+1);
			printf("%d. (%2d, %2d)", pivot++, ranY, ranX);
			
			break;
		}
	}
}
void createCheatFood()
{
	MapCheckArray[15][5] = 2;
	setCursurPos(30, 5);
	printf("♥");
}

int inGameActivity() // 게임의 전반적인 부분을 다루는 함수
{
	int InputValue = 0; // 입력한 키의 값
	int CurrentTime = 0; // Current Time
	createFood();
	int lastScore = 0;
	while (1) {
		PassedTime = clock();
		setCursurPos(70, 11);
		printf("%d:%2d", (PassedTime/1000)/60,(PassedTime/1000)%60);


		if (GameScore - lastScore > 5000) {
			IsFeverTime = true;
			setCursurPos(95, 24);
			printf("Fever Time!");
			FeverFactor = 2;
			SnakeSpeed = (int)(SnakeSpeed / FeverFactor);
			lastScore = GameScore;
		}
		if (IsFeverTime == true && GameScore - lastScore > 4000) {
			IsFeverTime = false;
			setCursurPos(95, 24);
			printf("             ");
			SnakeSpeed = (int)(SnakeSpeed*FeverFactor);
			FeverFactor = 1;
			lastScore = GameScore;
		}
		int ReferenceTime = clock(); // starts checking time with clock() function
		int ResOfAct = moveHead();
		if (ResOfAct == 0) {
			setCursurPos(30, 5);
			printf("Game Failed! RE? Y/N");
			break;
		}
		else if (ResOfAct == 3) {
			setCursurPos(30, 5);
			printf("You Win! RE? Y/N");
			break;
		}
		/*setCursurPos(0, 80); for debug
		printMap();
		system("pause");*/
		while (1)
		{
			if (kbhit()) { // if keyboard hit was detected,
				InputValue = getch();
				if (InputValue == ' ') {
					setCursurPos(95, 22);
					printf("Game Paused, Press Any Key to Continue");
					while (true) {
						if (kbhit())break;
					}
					setCursurPos(95, 22);
					printf("                                         ");
				}
				else setHeadDirection(InputValue);
			}
			CurrentTime += clock() - ReferenceTime; // 특정 시간이 지날때까지 대기
			if (CurrentTime >= SnakeSpeed) {
				CurrentTime = 0;
				break;
			}
			//break;
		}
	}
	return 0;
}

void initGameSettings() // 게임 초기 설정
{
	// 점수 표시 영역
	setCursurPos(70, 4);
	printf("Snake Length : ");
	setCursurPos(70, 5);
	printf("%d", BodyList.length);
	setCursurPos(70, 7);
	printf("Snake Speed");
	setCursurPos(70, 8);
	printf("%d", SnakeSpeed);
	setCursurPos(70, 10);
	printf("Time : ");
	setCursurPos(70, 11);
	printf("%d", 0);
	setCursurPos(70, 14);
	printf("Game Score : ");
	setCursurPos(70, 15);
	printf("              ");
	setCursurPos(70, 15);
	printf("%d", GameScore);
	setCursurPos(70, 17);
	printf("Bombs Exploded : ");
	setCursurPos(70, 18);
	printf("%d", BombCount);
	setCursurPos(70, 20);
	printf("Remain Count : ");
	setCursurPos(70, 21);
	printf("%d", RemainCount);
	setCursurPos(70, 23);
	printf("Food Count : ");
	setCursurPos(70, 24);
	printf("%3d", GatheredFood);
	setCursurPos(95, 20);
	printf("Game State");
	// 초기 뱀 위치 설정
	node.setX(15);
	node.setY(15);
	BodyList.BodySet[0] = node;

	node.setX(15);
	node.setY(16);
	BodyList.BodySet[1] = node;

	node.setX(15);
	node.setY(17);
	BodyList.BodySet[2] = node;

	MapCheckArray[15][15] = 1;
	MapCheckArray[15][16] = 1;
	MapCheckArray[15][17] = 1;

	printDot(BodyList.BodySet[0]);
	printDot(BodyList.BodySet[1]);
	printDot(BodyList.BodySet[2]);
}

void initGameValues() // 게임 변수 초기화
{
	for (int i = 0; i < HEIGHT; i++) { // 맵 초기화
		for (int j = 0; j < WIDTH; j++) {
			MapCheckArray[i][j] = 0;
		}
	}
	drawBorderLine(); // 맵 그리기
	BodyList.length = 3; // 초기 뱀 몸 길이 설정
	GatheredFood = 0; // 먹은 음식 수 초기화
	GameScore = 0; // 게임 점수 초기화
	SnakeSpeed = SPEED_FACTOR; // 뱀 속도 초기화
	CurrentHeadDirection = 0; // 뱀의 진행 방향 초기화
	BombCount = 0;
	pivot = 1;
	RemainCount = 0;
	PassedTime = 0;
	IsFeverTime = false;
	FeverFactor = 1;
}
int main()
{
	while (true) {
		BodyList.BodySet = (Node*)calloc(676, sizeof(Node)); // 26(=28-2) * 26개의 sizeof(Node) 만큼의 메모리 동적 할당 
		setCursurPos(0,0);
		initGameValues(); // 게임 변수 초기화
		initGameSettings(); // 게임 초기 설정
		inGameActivity(); // 실제 게임 실행

		free(BodyList.BodySet); // 메모리 해방
		setCursurPos(30, 6);
		char key;
		scanf(" %c", &key); // 재시작 여부 결정
		if(key == 'Y' || key == 'y') continue;
		else break;
	}
    return 0;
}

