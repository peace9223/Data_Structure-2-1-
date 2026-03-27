#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
<미로 만들기>
[1] 미로맵 랜덤으로 만들기 O
	-1. 10X10 으로 만들거임(가장자리에 출구와 입구를 제외한 나머지 모두가 1이니까 0이 들어갈 수 있는자리는 사실상 8X8)
	-2. 0과 1을 랜덤함수를 이용해 만들건데 10%확률로 1이 출력되도록 만듬

[2] 방향키 만들기 
	-1. 1이 있으면 그 쪽 방향으로 못감
	-2. 자신이 한 번 가더라도 뒤로 갈 수 있음
	-3. 현재 위치는 V로 표시

[3] 시작과 종료 만들기 O
	-1. 시작은 e로 표시 
	-2. 끝은 X로 표시 
	-3. 시작과 끝은 랜덤으로 정해짐

[4]  추가 점수 판 O
	-1. 맵에 2% 확률로 P라는 보너스가 등장 이때 P를 먹고 도착지까지 가면 추가점수 1점
	-2. 단) 맵에 여러개 나올수도 있고 한개도 안나올수도 있다
	-3. P를 먹게 되면 그 자리는 0으로 바뀜
*/
#define MAX_STACK_SIZE 100
#define MAX_SCORE_ARRAY_SIZE 100
#define MAZE 15

// -> Player 구조체
typedef struct
{
	char name[100];
	int level; // 플레이하는 단계
	int point; // 점수
	int scoreHistory[MAX_SCORE_ARRAY_SIZE]; // 플레이어 점수 히스토리(pop안함)
	int updownHistory[MAX_SCORE_ARRAY_SIZE];
	int top; // 점수 배열 확인용
	int top2; // 증감 배열 크기 확인용
}Player;

// -> 미로 구조체 만들기
typedef struct
{
	int x; // x 좌표
	int y; // y 좌표

}element; // 미로의 각각의 좌표

// -> 지나간길 저장하는 스택 구조체
typedef struct
{
	element mazeStack[MAX_STACK_SIZE]; // 지나간길 좌표 저장 
	int top;
}StackType;

//<플레이어 관련 함수>

// -> 플레이어 기본 정보 세팅
void InitPlayer(Player* p)
{
	p->point = 30; // 기본 포인트는 30점
	p->top = p->top2 = -1; // 점수가 쌓인 인덱스 저장
	p->level = 1;
	p->scoreHistory[++(p->top)] = 30; // 기본 점수 30 저장
	p->updownHistory[++(p->top2)] = 0;
	printf("\n\n닉네임을 적어주세요 : ");
	fgets(p->name, sizeof(p->name), stdin);
	for (int i = 0; i < strlen(p->name); i++)
	{
		if (p->name[i] == '\n')
		{
			p->name[i] = '\0';
			break;
		}
	}
	printf("\n%s 님 게임을 시작하겠습니다~!\n\n",p->name);
}

int Pis_Full(Player* p)
{
	return ((p->top == MAX_SCORE_ARRAY_SIZE - 1) || (p->top2 == MAX_SCORE_ARRAY_SIZE - 1)); // 배열의 크기가 100이니까 인덱스는 0~99 -> top이 99면 다 찬거임
}

void PPush(Player* p, int op, int np)
{
	// np(new point) : 증감이 완료된 현재 점수 저장  
	// op(old point) : 증감되기 전 점수 저장
	// op>np ==> 마이너스-> np - op , np>op ==> 플러스 --> np-op
	if (Pis_Full(p))
	{
		printf("OVERFLOW----더 이상 점수를 넣을 공간이 없습니다.. 개발자에게 문의해주세요~~\n");
	}
	else
	{
		p->updownHistory[++(p->top2)] = np-op; // 증감 점수 저장
		p->scoreHistory[++(p->top)] = np; // 증감 완료된 점수 저장
	}
}


//<STACK 만들기>

// -> init 기본 세팅 함수
void Init(StackType* s)
{
	s->top = -1;
}

// -> is_Full  용량이 가득 찼나요?
int is_Full(StackType* s)
{
	return (s->top == MAX_STACK_SIZE - 1);
}

// -> push 좌표를 집어넣는 함수
void Push(StackType* s, element item)
{
	if (is_Full(s))
	{
		printf("OVERFLOW(용량이 부족해서 게임을 종료합니다..\n");
		return;
	}
	else
	{
		s->mazeStack[++(s->top)] = item; // 스택의 첫번째 칸에 좌표 저장,, 구조체의 대입은 가능 하지만 비교는 불가
	}
}

// -> is_empty 더 이상 뺄게 없나요?
int is_Empty(StackType* s)
{
	return (s->top == -1);
}

// -> pop 가장 최신 스택 빼기 / 미로에서는 한칸 뒤로 물러나기 -> 반환(한칸 물러난 위치가 반환)
element pop(StackType* s)
{
	if (is_Empty(s))
	{
		printf("UNDERFLOW(뒤로 물러날 공간이 없어요)..\n");
		exit(1);
	}
	else
	{
		return s->mazeStack[s->top--];
	}
}

// -> 시작 위치, 종료 위치
element here; element end;

//<미로 맵 & 게임 운영 관련 함수>

// -> 미로 맵 만들기
char maze[MAZE][MAZE];
void CreateMaze(StackType *s)
{
	while (1)
	{
		
		int i, j;
		// -> 일단 맵의 모든 위치에 벽과 길을 생성
		for (i = 0; i < MAZE; i++) // 미로의 y축(행)
		{
			for (j = 0; j < MAZE; j++) // 미로의 x축에서 가장자리 벽을 제외한 구역에 벽을 세움
			{
				if (i == 0 || i == MAZE - 1 || j == 0 || j == MAZE - 1) // 가장자리는 모두 벽!
				{
					maze[i][j] = '1'; // 벽을 세운다
				}
				else
				{
					// char의 모든 요소들에게 0과 1을 부여
					int randMaze = rand() % 100 + 1; // randMaze가 1~100 중 하나이다.
					if (randMaze <= 17) maze[i][j] = '1'; // 벽 생성 확률 17%(난이도 조절)
					else
					{
						maze[i][j] = '0'; // 아니면 길 생성
						// -> 보너스~!!
						int bounsPoint = rand() % 100 + 1; // 랜덤박스 확률 설정 
						if (bounsPoint <= 15) maze[i][j] = 'R'; // 15% 확률로 보너스 P를 먹을 수 있는 기회를 줌
					}
				}
			}
		}
		// -> 시작위치와 끝 위치를 랜덤 생성
		int startPoint = rand() % (MAZE - 2) + 1; // startPoint는 MAZE-2 중 랜덤
		int endPoint = rand() % (MAZE - 2) + 1;  // endPoint도 MAZE-2 중 랜덤
		maze[startPoint][0] = 'e'; // 시작 포인트를 e
		maze[endPoint][MAZE - 1] = 'x'; // 끝 포인트를 x
		if (maze[startPoint][1] == '0' && maze[endPoint][MAZE - 2] == '0')
		{
			here.y = (int)startPoint; here.x = 0; // 시작 위치 저장 
			end.y = (int)endPoint; end.x = MAZE-1; // 끝 위치 저장 
			break;
		}
	}
	
}

void CreateMazeL2(StackType* s)
{
	while(1)
	{
		
		int i, j;
		// -> 일단 맵의 모든 위치에 벽과 길을 생성
		for (i = 0; i < MAZE; i++) // 미로의 y축(행)
		{
			for (j = 0; j < MAZE; j++) // 미로의 x축에서 가장자리 벽을 제외한 구역에 벽을 세움
			{
				if (i == 0 || i == MAZE - 1 || j == 0 || j == MAZE - 1) // 가장자리는 모두 벽!
				{
					maze[i][j] = '1'; // 벽을 세운다
				}
				else
				{
					// char의 모든 요소들에게 0과 1을 부여 1이될 확률이 10%확률(벽이 생성될 확률이 30%)
					int randMaze = rand() % 100 + 1; // randMaze가 1~100 중 하나
					if (randMaze <= 27) maze[i][j] = '1'; // randMaze가 10이면 벽 생성
					else
					{
						maze[i][j] = '0'; // 아니면 길 생성
						// -> 보너스~!!
						int bounsPoint = rand() % 100 + 1; // 랜덤박스 확률 설정 
						if (bounsPoint <= 25) maze[i][j] = 'R'; // 25% 확률로 보너스 P를 먹을 수 있는 기회를 줌
					}
				}
			}
		}
		// -> 시작위치와 끝 위치를 랜덤 생성
		int startPoint = rand() % (MAZE - 2) + 1; // startPoint는 MAZE-2 중 랜덤
		int endPoint = rand() % (MAZE - 2) + 1;  // endPoint도 MAZE-2 중 랜덤
		maze[startPoint][0] = 'e'; // 시작 포인트를 e
		maze[endPoint][MAZE - 1] = 'x'; // 끝 포인트를 x
		if (maze[startPoint][1] == '0' && maze[endPoint][MAZE - 2] == '0')
		{
			here.y = (int)startPoint; here.x = 0; // 시작 위치 저장 
			end.y = (int)endPoint; end.x = MAZE - 1;; // 끝 위치 저장 
			break;
		}
	}
}

void CreateMazeL3(StackType* s)
{
	while (1)
	{
		
		int i, j;
		// -> 일단 맵의 모든 위치에 벽과 길을 생성
		for (i = 0; i < MAZE; i++) // 미로의 y축(행)
		{
			for (j = 0; j < MAZE; j++) // 미로의 x축에서 가장자리 벽을 제외한 구역에 벽을 세움
			{
				if (i == 0 || i == MAZE - 1 || j == 0 || j == MAZE - 1) // 가장자리는 모두 벽!
				{
					maze[i][j] = '1'; // 벽을 세운다
				}
				else
				{
					// char의 모든 요소들에게 0과 1을 부여 1이될 확률이 10%확률(벽이 생성될 확률이 30%)
					int randMaze = rand() % 100 + 1; // randMaze가 1,2,3,4,5,6,7,8,9,10 중 하나이다.
					if (randMaze <= 35) maze[i][j] = '1'; // randMaze가 10이면 벽 생성
					else
					{
						maze[i][j] = '0'; // 아니면 길 생성
						// -> 보너스~!!
						int bounsPoint = rand() % 100 + 1; // 랜덤박스 확률 설정 
						if (bounsPoint <= 10) maze[i][j] = 'R'; // 5% 확률로 보너스 P를 먹을 수 있는 기회를 줌
					}
				}
			}
		}
		// -> 시작위치와 끝 위치를 랜덤 생성
		int startPoint = rand() % (MAZE - 2) + 1; // startPoint는 MAZE-2 중 랜덤
		int endPoint = rand() % (MAZE - 2) + 1;  // endPoint도 MAZE-2 중 랜덤
		maze[startPoint][0] = 'e'; // 시작 포인트를 e
		maze[endPoint][MAZE - 1] = 'x'; // 끝 포인트를 x
		if (maze[startPoint][1] == '0' && maze[endPoint][MAZE - 2] == '0') // e(출발지점)앞, x(도착지점)뒤에 1이있으면 막히니까 0으로 만들어줌
		{
			here.y = (int)startPoint; here.x = 0; // 시작 위치 저장 
			end.y = (int)endPoint; end.x = MAZE - 1; // 끝 위치 저장 
			break;
		}
	}
}

// -> 맵 출력함수
void PrintMaze(Player *p) // 맵 그리기
{
	printf("------미로 탈출 게임-[ <LEVEL %d> 남은 포인트 : %d ]\n",p->level,p->point);
	int i, j;
	printf("x좌표-> ");
	for (int k = 0; k < MAZE; k++)
	{
		if (k > 9)
			printf("%d", k);
		else
			printf("%d ", k);

	}
	printf("\n");
	for (i = 0; i < MAZE; i++)
	{
		printf("%d\t",i);
		for (j = 0; j < MAZE; j++) printf("%c ", maze[i][j]);
		printf("\n");
	}
	printf("-------------------------------------\n");
}

// -> 현재 플레이어의 위치 출력함수
void CurrentPosition(StackType *s)
{
	printf("\n [%d번째 시도]\n", s->top+1);
	printf("현재 플레이어의 위치는 [%d,%d] 입니다.(화면에 V로 표시됩니다)\n", s->mazeStack[s->top].x, s->mazeStack[s->top].y);
}

// -> 탈출 함수(성공적 탈출) 
void Victory(StackType *s, Player *p)
{
	printf("\n\n---------------------------------------------------------------\n");
	printf("\n                  [LEVEL %d] 탈출성공 ~~~!!!!!!\n", p->level); p->level++;
	printf("\n---------------------------------------------------------------");
	printf("\n--------------------------클리어 정보--------------------------\n\n");
	printf("총 %d번 시도 끝에 탈출했습니다!!\n", s->top);
	element result;
	int change = 0; // pop 값이 10이 넘어가면 줄바꾸기
	int ttop = s->top+1; // pop을 하면 top의 값이 줄어드니 미리 저장
	for (int i = 0; i < ttop; i++)
	{
		result = pop(s);
		printf("[%d,%d] ", result.x, result.y);
		if (!(i == ttop - 1))
			printf("<- ");
		change++;
		if (change % 10 == 0)
			printf("\n");
	}
	printf("\n");
	printf("\n[포인트 History]-----------------------------------------------\n");
	printf("[ 증감 , 점수 ]\n\n");
	if (p->top == p->top2)
	{
		for (int i = 0; i <= p->top; i++)
		{	
			if (i % 5 == 0)
				printf("[%4d,%4d]  ", p->updownHistory[i], p->scoreHistory[i]);
			
		}
		printf("\n");
		for (int i = 0; i <= p->top; i++)
		{
			if (i % 5 == 1)
				printf("[%4d,%4d]  ", p->updownHistory[i], p->scoreHistory[i]);
		}
		printf("\n");
		for (int i = 0; i <= p->top; i++)
		{
			if (i % 5 == 2)
				printf("[%4d,%4d]  ", p->updownHistory[i], p->scoreHistory[i]);
		}
		printf("\n");
		for (int i = 0; i <= p->top; i++)
		{
			if (i % 5 == 3)
				printf("[%4d,%4d]  ", p->updownHistory[i], p->scoreHistory[i]);
		}
		printf("\n");
		for (int i = 0; i <= p->top; i++)
		{
			if (i % 5 == 4)
				printf("[%4d,%4d]  ", p->updownHistory[i], p->scoreHistory[i]);
		}
		printf("\n");
	}
	if (p->level == 4)
	{
		printf("\n %s님 모든 레벨 클리어 축하드립니다!!!\n 최종점수는 : %d점 입니다!\n",p->name, p->scoreHistory[p->top]);
	}
	else
	{
		printf("\n 최종점수는 : %d점 입니다!\n", p->scoreHistory[p->top]);
	}
	printf("\n---------------------------------------------------------------\n");


}

// -> 게임 오버 함수
void GameOver(Player *p)
{
	if (p->point <= 0)
	{
		printf("\n--------------------------\n");
		printf("\n[ G A M E O V E R ]\n\n");
		printf(" 이동할 수 있는 포인트가 부족합니다ㅜㅜ\n\n");
		printf(" 최종 LEVEL : %d\n\n", p->level - 1);
		printf("\n--------------------------\n");
	}
	else
	{
		printf("\n--------------------------\n");
		printf("\n[ G A M E O V E R ]\n");
		printf("\n 최종 LEVEL : %d\n", p->level - 1);
		printf("\n--------------------------\n");
	}
}

// -> 랜덤박스 점수 출력
void PeekPoint(Player* p, int pp, int op)
{
	if (op > pp) printf("\n> %d점 마이너스 ! || 현재 점수는 %d점 입니다!<", op - pp, pp); // 점수 하락
	else if (op < pp) printf("\n> %d점 플러스 ! || 현재 점수는 %d점 입니다!<", pp - op, pp); // 점수 증가
	else printf("\n>0점 획득! || 현재 점수는 %d점 입니다!<", pp);

}

// -> 랜덤박스(랜덤 포인트)
void RandomBox(StackType *s,Player* p)
{
	/*
	[확률 공지]
	1.   10% : +5, +3, +2, -1, -3, - 5
	2.    5% : +10, -10
	3.    5% : 출발지 이동!
	4.    5% : 랜덤 좌표로 이동
	5.  0.1% : 이스터에그 +50 
	6. 18.9& : +0
	*/
	int x = s->mazeStack[s->top].x; // 스택의 맨 위 좌표의 x좌표
	int y = s->mazeStack[s->top].y; // 스택의 맨 위 좌표의 y좌표
	int originalpoint = p->point; // 원래 점수 저장
	int rate = rand() % 1000 + 1; // rate는 1~1000중 하나
	if (rate > 900) 
	{
		p->point += 5;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 800) 
	{
		p->point += 3;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 700)
	{
		p->point += 2;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 600) 
	{
		p->point -= 5;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 500)
	{
		p->point -= 3;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 400)
	{
		p->point -= 1;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 350)
	{
		printf("\n대박!~!\n");
		p->point -= 10;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 300)
	{
		printf("\n대박!~!\n");
		p->point += 10;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else if (rate > 250)
	{
		printf("\n--------------\n");
		printf("\n[ 출발지로 이동 !! ]\n");
		printf("\n--------------");
		maze[y][x] = '0';// 현재 자리에 v표시 삭제
		element stack;
		stack = here; // 출발지로 이동
		Push(s, stack);// 스택의 가장 윗 좌표 출발지점으로 바꿔줌(PPush)

	}
	else if (rate > 200)
	{
		printf("\n--------------\n");
		printf("\n[ 랜덤 좌표로 이동 !! ]\n");
		printf("\n--------------");
		maze[y][x] = '0';
		element stack; // 이동 좌표 저장
		while (1)
		{
			int RandomX = rand() % 13 + 1; //x좌표가 1~13 중 하나
			int RandomY = rand() % 13 + 1; //y좌표가 1~13 중 하나
			if (maze[RandomY][RandomX] == '0')
			{
				maze[y][x] = '0'; //현재 위치 0 표시
				maze[RandomY][RandomX] = 'V'; // 랜덤 좌표 V로 체크
				stack.x = RandomX; stack.y = RandomY;
				Push(s, stack); // 랜덤 좌표 이동
				break;
			}
			
		}
	}
	else if (rate == 1)
	{
		printf("\n이스터에그 발동!^^\n");
		p->point += 50;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
	else
	{
		p->point += 0;
		PPush(p, originalpoint, p->point);
		PeekPoint(p, p->point, originalpoint);
	}
}

// -> 현재 이동지 선택함수
int Move(StackType* s, Player* p)
{
	int choice; // Select 함수의 리턴값을 받음
	int x = s->mazeStack[s->top].x; // 스택의 맨 위 좌표의 x좌표
	int y = s->mazeStack[s->top].y; // 스택의 맨 위 좌표의 y좌표
	int originalPoint = p->point;
	choice = Select(s,p);
	if (choice == 10)
	{
		GameOver(p);
		return 3;
	}
	if (choice == 11)
		return 2;
	
	element next;
	next.x = 0, next.y = 0;
	if (maze[y][x] != 'e') // 시작점은 0으로 바뀔수 없음
	{
		maze[y][x] = '0'; // 원래있던 자리는 0으로 바꿈 그리고 이동한 자리에 'V' 표시
	}
	switch (choice)
	{
	case 4: // 아래쪽
		next.x = x;
		next.y = y + 1;
		break;
	case 3: // 왼쪽
		next.x = x - 1;
		next.y = y;
		break;
	case 2: // 오른쪽
		next.x = x + 1;
		next.y = y;
		break;
	case 1: // 위쪽
		next.x = x;
		next.y = y - 1;
		break;
	}
	Push(s, next); // next라는 좌표를 mazeStack에 푸시

	if (maze[next.y][next.x] == 'x')
	{
		p->point--;
		PPush(p, originalPoint, p->point);
		return 1; // 탈출했을 때 1을 반환
	}
	else if (maze[next.y][next.x] == 'R')
	{
		p->point--;
		PPush(p, originalPoint, p->point);
		RandomBox(s,p);
		if (p->point <= 0) // 포인트가 부족할 때
		{
			return 2;
		}
	}
	else
	{
		maze[next.y][next.x] = 'V'; // 맵에 V표시
		p->point--;
		PPush(p, originalPoint, p->point);
		if (p->point <= 0) // 포인트가 부족할 때
		{
			return 2;
		}
	}
	printf("\n\n");
	return 0;
}

// -> 현재 이동지 선택 함수 ( 번호를 받는 함수만 분리함)
int Select(StackType *s, Player *p)
{
	char select; //  방향키를 받는 변수
	int x = s->mazeStack[s->top].x; // 스택의 맨 위 좌표의 x좌표
	int y = s->mazeStack[s->top].y; // 스택의 맨 위 좌표의 y좌표
	while (1)
	{
		printf("\n** <게임 종료>를 원할시 'gameover'라고 입력해주세요! **");
		printf("\nW(위쪽), A(왼쪽), S(아래쪽), D(오른쪽) 입니다 영문자 하나만 입력해주세요! : ");

		// -> 입력값 오류 검사
		char go[20]; // 혹시 숫자말고 다른걸 입력을 할 경우를 대비
		scanf(" %s", go);

		if (strcmp(go, "gameover") == 0 || strcmp(go, "over") == 0 || strcmp(go, "'over'") == 0 || strcmp(go, "'gameover'") == 0)
		{
			return 10;
		}
		select = go[0]; // 일단 select가 1이라고 적으면 select에는 정수 1이 저장, 1번 이라고 적어도 해결
		//but 만약 첫글자가 범위에 맞는 숫자가 아니거나 한글로 적을시 오류

		if (select == 'w' || select == 'W')
		{
			if (maze[y - 1][x] == '0' || maze[y - 1][x] == 'x' || maze[y - 1][x] == 'R') 
				return 1; // 위쪽은 리턴 1
			else
			{
				printf("잘못된 방향입니다. 다시 입력해 주세요!\n");
			}
			
		}   // 위쪽은 리턴 1
		else if (select == 'd' || select == 'D')
		{
			if (maze[y][x + 1] == '0' || maze[y][x + 1] == 'x' || maze[y][x + 1] == 'R')
				return 2; // 오른쪽은 리턴 2
			else
			{
				printf("잘못된 방향입니다. 다시 입력해 주세요!\n");
			}
		}  
		else if (select == 'a' || select == 'A')
		{
			if (maze[y][x - 1] == '0' || maze[y][x - 1] == 'x' || maze[y][x - 1] == 'R')
				return 3; // 왼쪽은 리턴 3
			else
			{
				printf("잘못된 방향입니다. 다시 입력해 주세요!\n");
			}
		}	
		else if(select == 's' || select == 'S') // 숫자 범위인 경우
		{
			if (maze[y + 1][x] == '0' || maze[y + 1][x] == 'x' || maze[y + 1][x] == 'R')
				return 4; // 아래쪽은 리턴 4
			else
			{
				printf("잘못된 방향입니다. 다시 입력해 주세요!\n");
			}
		}
		else
		{
			printf("\n------------------Waring------------------\n");
			printf("문자 한글자만 입력해주세요!\n");
			printf("WASD를 제외한 문자는 입력하지 말아주세요!\n");
			printf("------------------------------------------\n\n");
		}
	}
}

// -> 게임 룰 설명
int Rule()
{
	char answer[20];
	printf("\n[ 게임 룰 ]\n");
	printf("\n1. 이 게임은 주어진 포인트를 적절히 사용하여 미로를 탈출하는 게임입니다\n");
	printf("\n2. 미로 맵의 단계는 총 3단계로 구성되어 있습니다. 처음에는 1단계 부터 시작하며 1단계 통과시 2단계가 시작됩니다.\n");
	printf("\n3. 맵은 15X15로 구성되어 있으며 플레이어는 'V'로 표시됩니다.(출발점('e')에 있을 때는 표시되지 않습니다!!)\n");
	printf("\n4. 출발점['e'], 도착 지점['x'], 벽(가지 못하는 길)['1'], 이동 가능한 길['0'], 랜덤박스(상호작용)['R'] 입니다.\n");
	printf("\n5. 랜덤박스에는 포인트를 주는 아이템, 출발지로 이동시키는 아이템, 랜덤 좌표로 이동시키는 아이템 그리고 이스터에그도 있습니다.\n");
	printf("\n6. 이동할 때마다 포인트가 소모됩니다. 포인트가 0이 되면 탈락입니다.\n");
	printf("\n7. 벽['1']이 랜덤으로 생기기 때문에 탈출 경로가 없을 수 도 있습니다. 그럴 경우를 대비해서 다시 하기를 만들어 놓았습니다!\n   (물론 랜덤박스에서 랜덤 좌표를 통한 대박을 노려도 됩니다^^) \n   * 다시하기는 경로가 없을때만 사용해주세요!게임의 재미가 반감이 됩니다 *\n");
	printf("\n -->> 간단하게 말하면 포인트를 적절히 사용해서 탈출을 하면 되는 게임입니다.\n");
	while (1)
	{
		printf("\n게임을 시작하시겠습니까?(yes/no) -->> ");
		scanf("%s", answer);
		if (strcmp(answer, "Y") == 0 || strcmp(answer, "y") == 0 || strcmp(answer, "Yes") == 0 || strcmp(answer, "yes") == 0
			|| strcmp(answer, "YES") == 0)
		{
			return 0;
		}
		else if (strcmp(answer, "N") == 0 || strcmp(answer, "n") == 0 || strcmp(answer, "No") == 0 || strcmp(answer, "no") == 0
			|| strcmp(answer, "NO")==0)
		{
			return 1;
		}
		else
		{
			printf("\n------------------Waring------------------\n");
			printf("\n(Yes/No) 둘 중에 하나만 적어주세요!!\n\n");
			printf("------------------------------------------\n\n");
		}
	}
}

//<게임 시작 관련 함수>

// -> Level 2 진입 하시겠습니까?
int GoLevel2()
{
	char quiz[10];
	while (1)
	{
		
		printf("\n [ 2단계로 넘어가시겠습니까?(네(넘어간다)....Y/아니요(게임을 종료한다)....N ] : ");
		scanf("%s", quiz);
		if (quiz[0] == 'Y' || quiz[0] == 'y') return 1;
		else if (quiz[0] == 'N' || quiz[0] == 'n') return 0;
		else printf(" [ 올바른 값을 입력해주세요! ]\n\n");
	}
}

// -> 레벨 2 설명
void Level2Tuto(Player* p)
{
	printf("%s님 레벨 %d에 오신걸 환영합니다!\n",p->name,p->level);
	printf("현재 포인트는 %d입니다. 이전 레벨을 통과 하셨기 때문에 15점을 추가로 드립니다!\n", p->point);
	p->point += 15;
	printf("레벨 2 시작포인트 : %d\n", p->point);
	printf("행운을 빕니다~~!!\n\n");
}

// -> 배열이랑 스택 초기화
void clear(StackType* s, Player* p)
{
	p->top = p->top2 = s->top =  -1;
	for (int i = 0; i < MAX_SCORE_ARRAY_SIZE; i++)
	{
		p->scoreHistory[i] = 0;
		p->updownHistory[i] = 0;
		s->mazeStack[i].x = 0;
		s->mazeStack[i].y = 0;
	} // top 초기화, 스택이랑 배열 초기화
}

// -> Level 3 진입 하시겠습니까?
int GoLevel3()
{
	char quiz[10];
	while (1)
	{
		printf("\n [ 3단계로 넘어가시겠습니까?(네(넘어간다)....Y/아니요(게임을 종료한다)....N ] : ");
		scanf("%s", quiz);
		if (quiz[0] == 'Y' || quiz[0] == 'y') return 1;
		else if (quiz[0] == 'N' || quiz[0] == 'n') return 0;
		else printf(" [ 올바른 값을 입력해주세요! ]\n\n");
	}
}

// -> 레벨 3 설명
void Level3Tuto(Player* p)
{
	printf("%s님 레벨 3에 오신걸 환영합니다!\n", p->name);
	printf("현재 포인트는 %d입니다. 이전 레벨을 통과 하셨기 때문에 15점을 추가로 드립니다!\n", p->point);
	p->point += 15;
	printf("레벨 3 시작포인트 : %d\n", p->point);
	printf("행운을 빕니다~~!!\n\n");
}

// -> 게임 리셋
int GameReset()
{
	char reset[10];
	int iReset;
	printf("\n[다시하기]\n");
	printf("1. 시작지점(e)과 탈출지점('x') 앞에 벽('1')로 막혀있다면 다시하기를 선택해주세요.\n");
	printf("2. 미로를 눈으로 보고 탈출 지점('x')에 갈 수 없다면 다시하기를 선택해 주세요.\n\n");
	while (1)
	{
		printf("다시 하시겠습니까??(다시하기...1 / 그대로 게임시작...0) >> ");
		scanf("%s", reset);
		printf("\n");
		iReset = reset[0] - '0';
		if (iReset == 1) // 다시하기
		{
			printf("--------------------------\n");
			printf("\n   <미로가 바뀌었습니다>\n\n");
			return 1;
		}
		else if (iReset == 0) return 0; // 그대로 게임 시작
		else
		{
			printf("\n------------------Waring------------------\n");
			printf("숫자만 입력해주세요!\n");
			printf("범위 밖의 숫자를 입력하지 말아주세요!\n");
			printf("------------------------------------------\n\n");
		}
	}

}

// -> 레벨 3 게임 시작
void GameStartLevel3(StackType* s, Player* p)
{
	int value = 0;
	int i = GoLevel3();
	if (i == 1)
	{
		Level3Tuto(p);
		clear(s, p);
		do
		{
			CreateMazeL3(s);
			PrintMaze(p);
		} while (GameReset());
		Push(s, here);
		while (1)
		{
			PrintMaze(p); // 화면에 미로 띄우고
			CurrentPosition(s); // 현재 위치 알려주고
			value = Move(s, p);
			if (value == 1) // 탈출을 했을 때 성공 문구 알려주고 게임 종료
			{
				Victory(s, p);
				break;
			}
			else if (value == 2) // 포인트가 부족할때, 게임 조건에 맞지 않을때
			{
				GameOver(p);
				break;
			}
			else if (value == 3)
				break;
		}
	}
	else if (i == 0)
	{
		GameOver(p);
	}
}

// -> 레벨 2 게임 시작
void GameStartLevel2(StackType* s, Player* p)
{
	int value = 0;
	int i = GoLevel2();
	if (i == 1)
	{
		Level2Tuto(p);
		clear(s, p);
		do
		{
			CreateMazeL2(s);
			PrintMaze(p);
		} while (GameReset());
		Push(s, here);
		while (1)
		{
			PrintMaze(p); // 화면에 미로 띄우고
			CurrentPosition(s); // 현재 위치 알려주고
			value = Move(s, p);
			if (value == 1) // 탈출을 했을 때 성공 문구 알려주고 게임 종료
			{
				Victory(s, p);
				GameStartLevel3(s, p);
				break;
			}
			else if (value == 2) // 포인트가 부족할때, 게임 조건에 맞지 않을때
			{
				GameOver(p);
				break;
			}
			else if (value == 3)
				break;
		}
	}
	else if (i == 0)
	{
		GameOver(p);
	}
}

// -> 게임 시작 함수
void GameStart(StackType* s, Player* p)
{
	int value = 0; // 1이면 탈출 성공, 2면 포인트 부족으로 탈출 실패, 3이면 gameover입력으로 인한 탈출 실패
	InitPlayer(p); // 플레이어 기본 정보 입력
	int gameRule = Rule();
	if (gameRule)
	{
		printf("\n\n게임을 종료합니다!\n");
		return;
	}
	printf("\n");
	do
	{
		Init(s);
		CreateMaze(s);
		PrintMaze(p);
	} while (GameReset());

	Push(s, here);
	while (1)
	{
		PrintMaze(p); // 화면에 미로 띄우고
		CurrentPosition(s); // 현재 위치 알려주고
		value = Move(s,p);
		if (value == 1) // 탈출을 했을 때 성공 문구 알려주고 게임 종료
		{
			Victory(s, p);
			GameStartLevel2(s, p);
			break;
		}
		else if (value == 2) // 포인트가 부족할때, 게임 조건에 맞지 않을때
		{
			GameOver(p);
			break;
		}
		else if (value == 3)
			break;
	}
	
}

int main()
{
	srand(time(NULL));
	Player p;
	StackType s;
	GameStart(&s, &p);
}

/*
아이디어
- 현재 위치는  maze[.y][.x] = 'V' 원래 있던자리는 다시 '0' 표시
*/