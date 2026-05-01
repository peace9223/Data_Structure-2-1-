/*
<연결리스트의 응용 다항식>
1. 지수만 저장한 배열
2. 가수를 저장한 배열
(굳이 2개의 배열을 만들 필요가 없이 가수가 있는 배열을 가지고 인덱스를 지수로 지정을 하면 된다)
(근데 x의 100제곱이면 중간에 항이 없는 즉 0을 가진 배열이 많이 나온다.)  -> 유효한 배열이 적다. -> 배열은 다항식으로 표현하기에 적합하지 않다.
-->> 링크드 리스트로 표현을 하자

<다항식 : 링크드 리스트>
가수 지수 링크 주소
p209 의 마지막 다항식 시험문제 연결리스트로 그릴수 있어야 한다.
11x¹² − 3x¹⁰ + 2x⁸ + 10x⁶ + 1
A = 3x¹² + 2x⁸ + 1
B = 8x¹² − 3x¹⁰ + 10x⁶
--------------------------
위와 아래의 지수를 비교해서 같으면 계산 다르면 큰것을 먼저 호출 상수는 x

a는 3 12 link -> 2 8 link -> 1 0 \
b는 8 12 link -> -3 10 link -> 10 6 \
c(New) 11 12 link -> -3 10 link -> 쭉
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct ListNode {
	int coef; // 가수
	int expon; // 지수
	struct ListNode* link;
}ListNode;

// 처음 a의 head의 노드에는 시작주소와 끝 노드의 주소를 넣고 마지막은 전체 노드의 개수를 넣었다.
// b도 마찬가지로 설정

typedef struct ListType {
	// 시작 주소 끝 주소 노드 개수
	int size; // 노드의 수
	ListNode* head; // 시작 주소 
	ListNode* tail; // 끝 주소
}ListType;
// a와 b의 덧셈 결과인 c도 마찬가지로 요소를 3가지 가지고 있어야한다.

//ListType 초기화 함수
ListType* create() {
	// 동적할당
	ListType* plist = (ListType*)malloc(sizeof(ListType));
	plist->size = 0;
	plist->head = NULL;
	plist->tail = NULL;
	return plist; 
}

void insert_last(ListType* s, int coef, int expon) {
	//listNode 메모리 할당
	ListNode* tmp = (ListNode*)malloc(sizeof(ListNode));
	// tmp에 값을 채워보자
	tmp->coef = coef;
	tmp->expon = expon;
	tmp->link = NULL; // 처음 노드는 끝과 시작의 주소가 같다. 
	// 첫번 쨰 노드인지 판단하기(매우 중요)
	if (s->head == NULL) {
		// 만족하면 첫번째 노드였다는 소리
		s->head = tmp; // 시작부분 tmp 연결
		s->tail = tmp; // 끝주소도 tmp연결

	}
	else {
		// 첫번째가 아닌경우
		// 끝 부분이 새로운 노드에 연결 밑 새로운 노드의 링크를 null로
		s->tail->link = tmp; // 이게 무조건 먼저 실행 되어야 한다. 즉 tail이 덮여써지면 안된다.  원래 노드의 tail에 새로운 노드 ㅇ녀결
		s->tail = tmp; // 순서 바뀌면 안됨  끝부분에 tmp 연결
	}
	s->size++;// 노드가 한개 더 연결되었음을 의미
}

void ploy_add(ListType* plist1, ListType* plist2, ListType* plist3) {
	// list1 + list2 = list3
	// 현재 list3는 NULL NULL 0로 저장
	// plist head를 쫒아가면 따라갈 수 있음
	ListNode* a = plist1->head; //a가 첫번째 노드를 가리키게끔 함
	ListNode* b = plist2->head;
	int sum; // 지수가 같을 때 가수를 더하는 임시 저장값
	while (a && b) { // a가 끝나거나 b가 끝나면 비교를 할 수 없기 때문에 불가능
		// 더하려면 비교해야함 지수끼리
		if (a->expon == b->expon) {
			sum = a->coef + b->coef; // 가수들을 더함
			if (sum != 0) { // 가수가 0이면 노드를 만들지 않는 상황임
				insert_last(plist3, sum, a->expon); // a와 b의 지수가 같기 떄문에 상관없다 아무거나 보내라
			}
			a = a->link;
			b = b->link;
		}
		else if (a->expon > b->expon) {
			insert_last(plist3, a->coef, a->expon);
			a = a->link; //else 문과는 반대의 결과 정확히 반대
		}
		else {
			// a의 지수가 더 작은 경우
			// 지수가 큰값을 보내기만 하면됨
			insert_last(plist3, b->coef, b->expon); // 지수가 큰 노드를 보냄
			b = b->link;
		}
	}
	// 더 긴 노드의 남은 값을 넣어주기만 하면 끝
	for (; a != NULL; a = a->link) {
		insert_last(plist3, a->coef, a->expon);
	}

	for (; b != NULL; b = b->link) {
		insert_last(plist3, b->coef, b->expon); // c의 결과를 출력하는 함수 프린트를 책에서 찾아보세요! 영상을 보세요!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
}


int main() {
	ListType* list1, * list2, * list3; // a,b,c
	// 각각의 list는 ListType의 요소를 가리키는 포인터 변수
	list1 = create(); // 초깃값설정 head null , tail null , size 0
	list2 = create();
	list3 = create();

	insert_last(list1, 3, 12); // 가수, 지수를 lsit1에 붙이자
	insert_last(list1, 2, 8);
	insert_last(list1, 1, 0); // 최종적으로 size가 3
	insert_last(list2, 8, 12);
	insert_last(list2, -3, 10);
	insert_last(list2, 10,6); 

	ploy_add(list1, list2, list3);
	
}