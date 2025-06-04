#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <osproject.h>
#include "system.h"
#include"command.h"

//  Stack 초기화
Stack* StackInit() {
    Stack* s = (Stack*)malloc(sizeof(Stack));
    if (s) s->TopNode = NULL;
    return s;
}

//  Stack 해제
void StackFree(Stack* s) {
    while (s && s->TopNode) {
        SNode* tmp = s->TopNode;
        s->TopNode = tmp->linknode;
        free(tmp);
    }
    free(s);
}

// 디렉토리 트리 초기화 (root 노드 생성)
DTree* DLoad() {
    DTree* tree = (DTree*)malloc(sizeof(DTree));
    TNode* root = (TNode*)malloc(sizeof(TNode));

    strcpy(root->name, "/");
    root->type = 'd';
    root->Parent = NULL;
    root->left = NULL;
    root->right = NULL;
    root->SIZE = 0;
    root->contentIndex=-1;
    
    // root 디렉토리의 권한 설정 (rwxrwxrwx)
    for (int i = 0; i < 9; i++) {
        root->permission[i] = 1;
    }
    
    // root 디렉토리의 소유자/그룹 설정
    root->UID = 0;  // root 사용자
    root->GID = 0;  // root 그룹

    update_node_time(root);

    tree->root = root;
    tree->current = root;

    return tree;
}

// 디렉토리 트리 해제
void FreeTree(DTree* tree) {
    if (!tree) return;
    // TODO: 전체 순회하며 모든 TNode 해제
    free(tree->root);
    free(tree);
}

//  사용자 목록 불러오기
UList* UserListLoad() {
    UList* list = (UList*)malloc(sizeof(UList));
    list->head = list->tail = list->current = NULL;
    list->tUID = 0;
    list->tGID = 0;
    return list;
}

//  로그인 처리
void Login(UList* users, DTree* tree) {
    // root 사용자 생성
    UNode* root = (UNode*)malloc(sizeof(UNode));
    strcpy(root->name, "root");
    strcpy(root->dir, "/");
    root->UID = 0;
    root->GID = 0;
    root->linknode = NULL;

    users->head = root;
    users->tail = root;
    users->current = root;  
}

//  사용자 목록 저장
void UserListSave(UList* users) {
    // 예시 저장 생략 가능
}

//  사용자 해제
void FreeUserList(UList* users) {
    if (users->current) free(users->current);
    free(users);
}

// 프롬프트 출력
void PrintHeader(DTree* tree, Stack* stack, UList* users) {
    // 현재 위치의 전체 경로를 얻기 위해 스택 사용
    TNode *cursor = tree->current;
    
    // 현재 경로가 root면 단순히 /만 표시
    if (cursor == tree->root) {
        printf("%s@/:~$ ", users->current->name);
        return;
    }
    
    // 경로를 스택에 쌓기
    while (cursor && cursor->Parent) {
        Push(stack, cursor->name);
        cursor = cursor->Parent;
    }
    
    // 프롬프트 출력 시작
    printf("%s@", users->current->name);
    
    // 경로 출력
    printf("/");
    while (!IsEmpty(stack)) {
        printf("%s", Pop(stack));
        if (!IsEmpty(stack)) printf("/");
    }
    printf(":~$ ");
}

