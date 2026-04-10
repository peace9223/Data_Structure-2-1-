#include <stdio.h>

typedef int element;
typedef struct ListNode
{
	element data;
	struct ListNode *link; // 위에서 아래로 내려오기 때문에 ListNode를 모른다
} ListNode; // 또한 자기참조 구조체라고 한다 (구조체안 자기 구조체)

int main() {
	ListNode a, b;
	ListNode* head = &a; // a의 주소를 저장(시작점)
	a.data = 10;
	b.data = 20;
	a.link = &b; // b의 주소를 저장
	b.link = NULL; // 리스트의 끝이므로 주소를 저장하지 않음
	printf("%d\n", a.data);
	printf("%d\n", head->data);
	printf("%d\n", head->link->data);
	return 0;
}