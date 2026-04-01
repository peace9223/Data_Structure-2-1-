#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_STACK_SIZE 10
// Let's implement the stack first
typedef char element;

typedef struct
{
	int top; // top index
	element data[MAX_STACK_SIZE];

}StackType;

void Init(StackType* s)
{
	s->top = -1;
}

int IsEmpty(StackType* s)
{
	return (s->top == -1);
}

int IsFull(StackType* s)
{
	return s->top == MAX_STACK_SIZE - 1;
}

element Pop(StackType* s)
{
	if (IsEmpty(s))
	{
		printf("Underflow\n");
		exit(1);
	}

	return s->data[s->top--];
}

void Push(StackType* s, element n)
{
	if (IsFull(s))
	{
		printf("Overflow\n");
		return;
	}
	s->data[++(s->top)] = n;
}

element Peek(StackType* s)
{
	return s->data[s->top];
}

//convert Postfix -> Infix
int eval(const char str[])
{
	/* 
	<LOGIC>
	1. if str[i] is the number, Push onto the stack
	2. if str[i] is '+','-','/','*', Pop two values, culculator the result, push it back onto the stack
	3. if finish, Pop stack!
	*/
	// strlen = return length of string(-'\0')
	// size of = return the size of bytes(+'\0')
	int length = strlen(str);
	int i = 0;
	int value = 0, op1 = 0, op2 = 0;
	char ch;
	StackType s;
	Init(&s);
	for (i = 0; i < length; i++)
	{
		ch = str[i];
		if (ch != '+' && ch != '-' && ch != '/' && ch != '*')
		{
			value = ch - '0'; // value type is int, ch type is char
			Push(&s, value);
		}
		else
		{
			op1 = Pop(&s);
			op2 = Pop(&s);
			switch (ch)
			{
			case '+': Push(&s, op2 + op1); break;
			case '-': Push(&s, op2 - op1); break;
			case '/': Push(&s, op2 / op1); break;
			case '*': Push(&s, op2 * op1); break;
			}
		}
	}
	return Pop(&s); // final score!
}

int prec(char ch)
{
	switch (ch)
	{
	case '(':case ')': return 0;
	case '*':case '/': return 2;
	case '+':case '-': return 1;
	}
	return -1;
}

//convert Infix -> Postfix
void infixToPostfix(const char str[])
{
	int i = 0;
	int length = strlen(str);
	char ch, topOp;
	StackType s;
	Init(&s);
	/*
	1. 일단 숫자면 2스택에 저장
	2. 짝괄호가 나오면 원래 괄호가 나올 때까지 pop
	3. 기호는 1스택에 저장
	4. 상위 기호일 경우 
	*/
	/*
	<case>
	1. '(' 괄호 일 때 -> 전부 Push
	2. ')' 괄호 일 때 -> '(' 나올 때 까지 POP
	3. '+', '-', '*', '/' 일 때 -> 스택에 우선순위가 높거나 같은게 있으면 다 pop후 저장, 아니면 그냥 저장
	4. 기호가 아니다 숫자이다 -> Push
	*/
	for (i = 0; i < length; i++)
	{
		ch = str[i];
		switch (ch)
		{
		case '(': Push(&s, ch); break;  
		case ')': 
		{
			topOp = Pop(&s); // 일단 하나를 빼놓기
			while (topOp != '(')
			{
				printf("%c", topOp);
				topOp = Pop(&s);
			}
			break;
		}
		case '+':case '-':case '*': case '/':
		{
			while (!IsEmpty(&s) && prec(ch) <= prec(Peek(&s))) // 스택의 안의 기호가 우선순위가 높다면
			{
				printf("%c", Pop(&s));// 우선순위 높은 연산자 먼저 Pop하고 스택에 집어넣기(우선순위 같아도 실행)
			}
			Push(&s, ch);
			break;
		}
		default: // 숫자 일 때
			printf("%c", ch);
			break;
		}
	}
	while (!IsEmpty(&s))
		printf("%c",Pop(&s));
}

int main()
{
	int result = 0;
	
	infixToPostfix("(3+4)*5");
	
	return 0;
}