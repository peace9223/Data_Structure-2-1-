#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STACK_SIZE 5
// 사이즈가 5이다라는 말은 괄호가 쌍으로 총 5개까지 저장 가능!

/* 
 <중요중요중요 시험문제>
 <스택응용 괄호 검사문제> ⭐⭐
 1. 왼쪽 괄호의 개수와 오른쪽 괄호의 개수가 같아야한다.
 2. 같은 괄호에서 왼쪽 괄호는 오른쪽 괄호보다 먼저 나와야한다.
 3. 괄호 사이에는 포함 관계만 존재한다.
*/

// sizeof 와 strlen의 차이 strlen은 문자열 끝에 널문자를 포함하지 않는다
// in도 배열의 첫번째 주소를 가지고 있다.
// string.h -> strlen
// define 뒤에 세미콜론 안붙이는거 명심!!!
// typedef를 안쓰면 struct StackType이라고 써야함
// switch 문 이후에 break 반드시 쓰기!!!!!!@
// open_ch 와 ch 여는 괄호를 가지고 오는지 아니면 닫는 괄호를 가지고 오는지 중요
// and(&&), or(||) 조건 중요
typedef char element; // 의미상 int == element
typedef struct
{
	element data[MAX_STACK_SIZE];
	int top;
}StackType;
void init_stack(StackType* s) // 구조체 변수를 주소값으로 전달
{
	s->top = -1; // 구조체 변수 s의 top을 -1로 초기화
}
int is_empty(StackType* s)
{
	return (s->top == -1); // top이 -1 이면 스택이 빈것임
}
int is_full(StackType* s)
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
element pop(StackType* s)
{
	if (is_empty(s))
	{
		printf("스택 공백 에러(underflow)\n");
		exit(1); // pop함수는 반환을 해야하는데 return을 쓰면 이상한 값이 반환됨 즉 stdlib.h 라이브러리 안에 있는 exit(1)을 사용하자

	}
	else
		return s->data[s->top--];
}
element peek(StackType* s)
{
	if (is_empty(s))
	{
		printf("스택 부족 에러\n");
		exit(1);
	}
	else
		return s->data[s->top];
}
int check_matching(const char* in)
{
	StackType s;
	char ch, open_ch;
	int n = strlen(in);
	init_stack(&s);
	for (int i = 0; i < n; i++)
	{
		ch = in[i]; // {
		switch (ch) // ch가 괄호 기호라면 push
		{
		case '(':
		case '{':
		case '[':
			push(&s, ch); // 여는 괄호 기호라면 푸시
			break; 
		case ')':case '}':case ']': //가로도 가능능@!@
			if (is_empty(&s)) // 스택이 비어있으면 안된다. 스택이 비어있으면 empty문 자체에서 1을 반환
				return 0; // 우리는 오류가 났을 때 0을 반환한다.
			open_ch = pop(&s);
			//여는 괄호와 닫는 괄호의 모양이 같지 않을 때
			if (open_ch != '(' && ch == ')' || open_ch != '[' && ch == ']' || open_ch != '{' && ch == '}')
			{ // ❤️❤️ 여기 open_ch와 ch 위치 중요
				return 0; // 수식이 틀렸어요!!!!!!!!!!
			}
			break;
		}// end switch
	} // end for
	if (!is_empty(&s)) // 비어있지 않다면 ! (비어 있지 않으면 empty가 0을 반환)
	{
		return 0;
	}
	return 1; // 이 수식이 올바른 수식 입니다.
} // end check_matching
// 하지만 우리가 여기서 하나더 확인해야할것이 있다 
// for 문이 다 끝났는데 팝할께 남아있다면 즉 괄호가 남아있다면 !

int main(void)
{
	const char* p = "{ A[(i+1)]=0;}"; // 올바른 괄호형식
	
	if (check_matching(p) == 1)
	{
		printf("%s 괄호성공\n", p);
	}
	else
	{
		printf("%s 괄호검사실패\n",p);
	}
}