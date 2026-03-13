#include <stdio.h>
#include <stdlib.h>
#define MAX_STACK_SIZE 5

typedef int element; // 의미상 int == element
typedef struct
{
	element data[MAX_STACK_SIZE];
	int top;
}StackType;

void init_stack(StackType *s) // 구조체 변수를 주소값으로 전달
{
	s->top = -1; // 구조체 변수 s의 top을 -1로 초기화
}

int is_empty(StackType *s)
{
	return (s->top == -1); // top이 -1 이면 스택이 빈것임
}

int is_full(StackType *s)
{
	return (s->top == (MAX_STACK_SIZE - 1)); // 전체 크기가 5이면 top이 0 1 2 3 4 인덱스를 가짐
}

void push(StackType* s, element item)
{
	if (is_full(s))
	{
		printf("스택 포화 에러(overflow error)\n");
		return;
	}
	else
	{
		s->data[++(s->top)] = item;  // s->++top 이거 아님
	}
	
}


element pop(StackType *s)
{
	if (is_empty(s))
	{
		printf("스택 공백 에러(underflow)\n");
		exit(1); // pop함수는 반환을 해야하는데 return을 쓰면 이상한 값이 반환됨 즉 stdlib.h 라이브러리 안에 있는 exit(1)을 사용하자
	}
	else
		return s->data[s->top--];
}

element peek(StackType *s)
{
	if (is_empty(s))
	{
		printf("스택 부족 에러\n");
		exit(1);
	}
	else
		return s->data[s->top];
}

int main(void)
{
	
	StackType s; //구조체 변수
	
	init_stack(&s); // 주소를 전달
	push(&s, 10);
	push(&s, 20);
	push(&s, 30);

	int res = pop(&s);
	printf("pop value : %d\n", res);
	printf("pop value : %d\n", pop(&s));

}