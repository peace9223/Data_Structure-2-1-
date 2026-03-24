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
#define MAX_STACK_SIZE 50
#define MAZE 10

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
		return 0;
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

// ->미로 맵 만들기
char maze[MAZE][MAZE];
void CreateMaze(StackType *s)
{
	srand(time(NULL));
	int i, j;
	// -> 일단 맵의 모든 위치에 벽과 길을 생성
	for (i = 0; i < 10; i++) // 미로의 y축(행)
	{
		for (int j = 0; j < 10; j++) // 미로의 x축에서 가장자리 벽을 제외한 구역에 벽을 세움
		{
			if (i == 0 || i == 9 || j == 0 || j == 9) // 가장자리는 모두 벽!
			{
				maze[i][j] = '1'; // 벽을 세운다
			}
			else
			{
				// char의 모든 요소들에게 0과 1을 부여 1이될 확률이 10%확률(벽이 생성될 확률이 30%)
				int randMaze = rand() % 10 + 1; // randMaze가 1,2,3,4,5,6,7,8,9,10 중 하나이다.
				if (randMaze == 10) maze[i][j] = '1'; // randMaze가 10이면 벽 생성
				else
				{
					maze[i][j] = '0'; // 아니면 길 생성
					// -> 보너스~!!
					int bounsPoint = rand() % 100 + 1; // 보너스 포인트의 수는 1~100중 하나
					if (bounsPoint < 2) maze[i][j] = 'P'; // 2% 확률로 보너스 P를 먹을 수 있는 기회를 줌
				}
			}				
		}
	}
	// -> 시작위치와 끝 위치를 랜덤 생성
	int startPoint = rand() % 8 + 1; // startPoint는 1~8 중 랜덤
	int endPoint = rand() % 8 + 1;  // endPoint도 1~8 중 랜덤
	maze[startPoint][0] = 'e'; // 시작 포인트를 e
	maze[endPoint][9] = 'x'; // 끝 포인트를 x
	here.y = (int)startPoint; here.x = 0; // 시작 위치 저장 
	end.y = 0; end.x = (int)endPoint; // 끝 위치 저장 
}

// -> 맵 출력함수
void PrintMaze() // 맵 그리기
{
	printf("------미로 탈출 게임------\n");
	int i, j;
	printf("x좌표-> 0123456789\n");
	for (i = 0; i < MAZE; i++)
	{
		printf("%d\t",i);
		for (j = 0; j < MAZE; j++) printf("%c", maze[i][j]);
		printf("\n");
	}
	printf("--------------------------\n");
}

// -> 현재 플레이어의 위치 출력함수
void CurrentPosition(StackType *s)
{
	printf("현재 플레이어의 위치는 [%d,%d] 입니다.(화면에 V로 표시됩니다)\n", s->mazeStack[s->top].x, s->mazeStack[s->top].y);
}

// -> 이동지 선택 함수
void Move(StackType* s)
{
	int select;
	int dir[4];
	int x = s->mazeStack[s->top].x;
	int y = s->mazeStack[s->top].y;
	int count = 0; // 숫자 표시 함수
	if (maze[y+1][x]=='0')
	{
		dir[count++] = 1;
		printf("(%d) 아래쪽 ", count);
	}
	if (maze[y][x-1]=='0')
	{
		dir[count++] = 2;
		printf("(%d) 왼쪽 ", count);
	}
	if (maze[y][x+1]=='0')
	{
		dir[count++] = 3;
		printf("(%d) 오른쪽 ", count);
	}
	if (maze[y-1][x]=='0')
	{
		dir[count++] = 4;
		printf("(%d) 위쪽 ", count);
	}
	if (count == 0)
	{
		printf("갈수 있는 방향이 없습니다.. 게임을 종료합니다!\n");
		return 0;
	}
	printf("\n>> 어느 쪽을 선택 하시겠습니까?(번호만 적어주세요) : ");
	scanf_s("%d", &select);

	element next;
	if (maze[y][x] != 'e')
	{
		maze[y][x] = '0';
	}
	switch (dir[select-1])
	{
	case 1: // 아래쪽
		next.x = x;
		next.y = y + 1;
		break;
	case 2: // 왼쪽
		next.x = x - 1;
		next.y = y;
		break;
	case 3: // 오른쪽
		next.x = x + 1;
		next.y = y;
		break;
	case 4: // 위쪽
		next.x = x;
		next.y = y - 1;
		break;
	}
	Push(s, next);
	maze[next.y][next.x] = 'V';
	count = 0;
	printf("\n\n");
}

// -> 게임 시작 함수
void GameStart(StackType* s)
{
	Init(s);
	CreateMaze(s);
	Push(s, here);
	while(1)
	{
		PrintMaze(); // 화면에 미로 띄우고
		CurrentPosition(s); // 현재 위치 알려주고
		Move(s); // 움직이기!
	}
}

int main()
{
	StackType s;
	GameStart(&s);
	
	
}

/*
아이디어
- 현재 위치는  maze[.y][.x] = 'V' 원래 있던자리는 다시 '0' 표시
*/