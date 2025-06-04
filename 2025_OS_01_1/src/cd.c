
#include "command.h"
#include <string.h>
#include <stdio.h>
// /* ────────────────────────────────────────────────────────────────────────── */
// /* cd 본체                                                                  */
// /* ────────────────────────────────────────────────────────────────────────── */


int command_cd(DTree *dTree, UList* users, const char *command)
{
    char  tmpPath[MAXDIRECTORY];
    int   check;
    TNode *tmp;

    /* 옵션 처리 ----------------------------------------------------------- */
    if (command && command[0] == '-') {
        if (strcmp(command, "--help") == 0) {
            puts("Usage: cd [directory]");
            puts("Change the shell working directory.");
            return 0;
        }
        printf("cd: invalid option -- '%s'\n", command);
        puts("Try 'cd --help' for more information.");
        return -1;
    }

    /* 인수가 없으면 홈(=프로젝트 규칙상 root)으로 이동 -------------------- */
    if (!command) {
        dTree->current = dTree->root;
        UpdateUserDir(users, dTree);
        return 0;
    }

    /* 디렉터리 존재·권한·파일 여부 확인 ---------------------------------- */
    tmp = ExistDir(dTree, command, 'd');
    if (tmp && IsPermission(tmp, 'r') != 0) {
        printf("cd: %s: Permission denied\n", command);
        return -1;
    }
    if (ExistDir(dTree, command, 'f')) {
        printf("cd: %s: Not a directory\n", command);
        return -1;
    }

    /* 실제 이동 ----------------------------------------------------------- */
    strncpy(tmpPath, command, MAXDIRECTORY - 1);
    tmpPath[MAXDIRECTORY - 1] = '\0';
    check = MovePath(dTree, tmpPath);
    if (check != 0) {
        printf("cd: %s: No such file or directory\n", command);
        return check;
    }

    UpdateUserDir(users, dTree);
    return check;
}

/* ────────────────────────────────────────────────────────────────────────── */
/* 경로 파싱: 절대·상대 경로를 current 로 반영                             */
/* ────────────────────────────────────────────────────────────────────────── */
int MovePath(DTree *dTree, const char *path)
{
    TNode *backup = dTree->current;
    char  *seg;

    /* '/' 만 입력 → 루트 */
    if (strcmp(path, "/") == 0) {
        dTree->current = dTree->root;
        return 0;
    }

    /* 절대경로면 root 에서 시작 */
    if (path[0] == '/') {
        dTree->current = dTree->root;
        seg = strtok(path + 1, "/");
    } else {
        seg = strtok(path, "/");
    }

    while (seg) {
        if (Movecurrent(dTree, seg) != 0) {
            dTree->current = backup;         /* 실패 시 롤백 */
            return -1;
        }
        seg = strtok(NULL, "/");
    }
    return 0;
}

/* 한 단계 이동 ("." ".." 디렉터리 이름) ---------------------------------- */
int Movecurrent(DTree *dTree, const char *segment)
{
    if (strcmp(segment, ".") == 0)
        return 0;

    if (strcmp(segment, "..") == 0) {
        if (dTree->current != dTree->root)
            dTree->current = dTree->current->Parent;
        return 0;
    }

    TNode *dest = ExistDir(dTree, segment, 'd');
    if (!dest)
        return -1;

    dTree->current = dest;
    return 0;
}

/* ────────────────────────────────────────────────────────────────────────── */
/* 사용자의 dir 문자열 갱신                                                 */
/* ────────────────────────────────────────────────────────────────────────── */
static void build_path_rec(TNode *node, char *buf, size_t size)
{
    if (!node) return;
    if (node->Parent)                     /* root 이전에 재귀 호출 */
        build_path_rec(node->Parent, buf, size);

    if (node->Parent) {                   /* root 자체엔 /만 출력 */
        strncat(buf, "/", size - strlen(buf) - 1);
        strncat(buf, node->name, size - strlen(buf) - 1);
    } else {
        strncat(buf, "/", size - 2);      /* root */
    }
}

void UpdateUserDir(UList *users, DTree *tree)
{
    if (!users || !users->current) return;

    char path[MAXDIRECTORY] = {0};
    build_path_rec(tree->current, path, sizeof(path));
    strncpy(users->current->dir, path, MAXDIRECTORY);
}
