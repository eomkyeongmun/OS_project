#include "osproject.h"
#include "command.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 256

extern char* fileContentList[MAX_LINES];  // 다른 파일에서 초기화되어야 함

//─────────────────────────────────────────────────────────────────────────────
// 현재 디렉토리(tree->current) 바로 아래에서
// 이름이 name인 파일 노드를 찾아 반환 (없으면 NULL)
//─────────────────────────────────────────────────────────────────────────────
static TNode* findNodeInCurrent(DTree* tree, const char* name) {
    if (!tree || !tree->current) return NULL;

    TNode* child = tree->current->left;
    while (child) {
        if (strcmp(child->name, name) == 0 && child->type == 'f')
            return child;
        child = child->right;
    }
    return NULL;
}

//─────────────────────────────────────────────────────────────────────────────
// 문자열 끝에 '\r'이 있으면 제거하는 헬퍼 함수
//─────────────────────────────────────────────────────────────────────────────
static void trim_cr(char* line) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\r') {
        line[len - 1] = '\0';
    }
}

//─────────────────────────────────────────────────────────────────────────────
// diff 명령어: 현재 디렉토리에서 name1, name2 두 파일을 찾아
// 내용(줄 단위)을 비교하여 차이가 있으면 화면에 출력
//─────────────────────────────────────────────────────────────────────────────
void command_diff(DTree* tree, const char* name1, const char* name2) {
    if (!tree || !tree->current) {
        printf(" File system is not initialized.\n");
        return;
    }

    // 1) 두 파일 노드를 찾기
    TNode* file1 = findNodeInCurrent(tree, name1);
    TNode* file2 = findNodeInCurrent(tree, name2);

    if (!file1 || !file2) {
        printf(" One or both files not found.\n");
        return;
    }

    // 2) 읽기 권한 확인
    if (IsPermission(file1, 'r') != 0 || IsPermission(file2, 'r') != 0) {
        printf(" Permission denied: One or both files are not readable.\n");
        return;
    }

    // 3) SIZE 인덱스 범위 검사
    if (file1->contentIndex < 0 || file1->contentIndex >= MAX_LINES ||
        file2->contentIndex < 0 || file2->contentIndex >= MAX_LINES) {
        printf(" Invalid file index.\n");
        return;
    }

    // 4) fileContentList 인덱스 유효성 검사
    if (fileContentList[file1->contentIndex] == NULL || fileContentList[file2->contentIndex] == NULL) {
        printf(" One or both files are empty (no content).\n");
        return;
    }

    // 5) 파일 내용을 strdup 후 줄 단위로 분리
    char* dup1 = strdup(fileContentList[file1->contentIndex]);
    char* dup2 = strdup(fileContentList[file2->contentIndex]);
    if (!dup1 || !dup2) {
        printf("Memory allocation failed.\n");
        free(dup1);
        free(dup2);
        return;
    }

    char* lines1[MAX_LINES];
    char* lines2[MAX_LINES];
    int count1 = 0, count2 = 0;

    // 첫 번째 파일 줄 분리 및 '\r' 제거
    char* tok = strtok(dup1, "\n");
    while (tok && count1 < MAX_LINES) {
        // strdup 한 뒤, 끝에 남은 '\r'이 있으면 지운다
        char* line = strdup(tok);
        trim_cr(line);
        lines1[count1++] = line;
        tok = strtok(NULL, "\n");
    }

    // 두 번째 파일 줄 분리 및 '\r' 제거
    tok = strtok(dup2, "\n");
    while (tok && count2 < MAX_LINES) {
        char* line = strdup(tok);
        trim_cr(line);
        lines2[count2++] = line;
        tok = strtok(NULL, "\n");
    }

    // 6) 두 파일 중 더 큰 줄 수만큼 비교하며 차이점 출력
    int maxLines = (count1 > count2) ? count1 : count2;
    for (int i = 0; i < maxLines; i++) {
        const char* l1 = (i < count1) ? lines1[i] : "";
        const char* l2 = (i < count2) ? lines2[i] : "";
        if (strcmp(l1, l2) != 0) {
            printf("Line %d differs:\n", i + 1);
            printf("  %s\n", l1);
            printf("  %s\n", l2);
        }
    }

    // 7) 메모리 해제
    for (int i = 0; i < count1; i++) free(lines1[i]);
    for (int i = 0; i < count2; i++) free(lines2[i]);
    free(dup1);
    free(dup2);
}