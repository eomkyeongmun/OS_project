// 사용자 정보
#ifndef OSPROJECT_H
#define OSPROJECT_H

#define MAXN 64
#define MAXD 256
#define MAX_ALIAS 20
#define MAXDIRECTORY 256
#define MAXNAME 100

#include <time.h>

// 사용자 정보
typedef struct tagUN {
    char name[MAXN];
    char dir[MAXD];
    int year, month, wday, day, hour, minute, sec;
    int UID, GID;
    struct tagUN* linknode;
} UNode;

typedef struct tagUser {
    int tUID;
    int tGID;
    UNode* head;
    UNode* tail;
    UNode* current;
} UList;

// 디렉토리 트리
typedef struct tagTN {
    char name[MAXN];
    char type;
    int mode;
    int permission[9];
    int SIZE;
    int contentIndex; 
    int UID, GID;
    int month, day, hour, minute;
    struct tagTN* Parent;
    struct tagTN* left;
    struct tagTN* right;
} TNode;

typedef struct tagDTree {
    TNode* root;
    TNode* current;
} DTree;

// 스택
typedef struct tagSNode {
    char name[MAXN];
    struct tagSNode* linknode;
} SNode;

typedef struct tagStack {
    SNode* TopNode;
} Stack;

// alias
typedef struct tagAlias {
    char name[32];
    char command[128];
} Alias;

typedef struct tagAliasTable {
    Alias list[MAX_ALIAS];
    int count;
} AliasTable;

#endif
