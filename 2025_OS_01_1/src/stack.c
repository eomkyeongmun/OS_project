#include "stack.h"
#include <stdlib.h>
#include <string.h>

void Push(Stack *s, const char *str) {
    SNode* newNode = (SNode*)malloc(sizeof(SNode));
    if (!newNode) return;
    strncpy(newNode->name, str, MAXN - 1);
    newNode->name[MAXN - 1] = '\0';
    newNode->linknode = s->TopNode;
    s->TopNode = newNode;
}

char* Pop(Stack *s) {
    if (!s->TopNode) return "";
    SNode* node = s->TopNode;
    s->TopNode = node->linknode;
    return node->name;  // NOTE: 호출한 쪽에서 복사하거나 사용 후 free 필요
}

int IsEmpty(const Stack *s) {
    return s->TopNode == NULL;
}
