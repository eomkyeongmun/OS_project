#include "command.h"
#include <stdlib.h>
#include <string.h>
#include<time.h>
#include "parser.h" 


#define MAXPATH 1024          //추가cp에서 전역으로 쓰는 경로 길이 상수

void update_node_time(TNode* node) {
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    node->month = timeinfo->tm_mon + 1;
    node->day = timeinfo->tm_mday;
    node->hour = timeinfo->tm_hour;
    node->minute = timeinfo->tm_min;
}

/* 이미 구현된 삭제 함수 재사용 ----------------------------- */
void DeleteFile(DTree* tree, char* name)
{
    RemoveDir(tree, name);
}

/* 새 디렉터리(또는 파일 노드) 생성 ------------------------- */
void MakeDir(DTree* tree, char* name, char type)
{
    TNode* newNode = malloc(sizeof(TNode));
    if (!newNode) return;

    strncpy(newNode->name, name, MAXN - 1);
    newNode->name[MAXN - 1] = '\0';
    newNode->type   = type;
    newNode->left   = NULL;
    newNode->right  = tree->current->left;
    newNode->Parent = tree->current;   // 추가 

    if (type == 'd') { // 디렉토리인 경우
        newNode->SIZE = 4096;
        newNode->contentIndex = -1;
    } else { // 파일인 경우
        newNode->SIZE = 0; // 초기 파일 크기는 0
        newNode->contentIndex = -1; // cat 명령으로 내용이 추가될 때 인덱스 할당
    }

    update_node_time(newNode);

    tree->current->left = newNode;
}
int IsPermission(TNode* node, char mode) {
    if (!node) return -1;
    if (mode == 'r') return node->permission[0] ? 0 : -1;
    if (mode == 'w') return node->permission[1] ? 0 : -1;
    if (mode == 'x') return node->permission[2] ? 0 : -1;
    return -1;
}

