#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osproject.h"
#include "command.h"

int create_parent_directories(DTree* tree, const char* fullPath, int mode, UList* users) {
    char pathCopy[MAXD];
    strncpy(pathCopy, fullPath, MAXD);
    pathCopy[MAXD - 1] = '\0';

    // 경로를 각 부분으로 분리
    char* parts[MAXD];
    int count = 0;
    
    // 첫 번째 토큰
    char* token = strtok(pathCopy, "/");
    while (token && count < MAXD) {
        parts[count++] = token;
        token = strtok(NULL, "/");
    }

    // 각 깊이별로 경로 생성
    for (int i = 0; i < count; i++) {
        if (!ExistDir(tree, parts[i], 'd')) {
            MakeDir(tree, parts[i], 'd');
            TNode* newNode = ExistDir(tree, parts[i], 'd');
            if (newNode) {
                for (int j = 0; j < 9; j++) {
                    newNode->permission[j] = (mode >> (8 - j)) & 1;
                }
                // 현재 사용자의 UID/GID 설정
                newNode->UID = users->current->UID;
                newNode->GID = users->current->GID;
            }
        }
        // 생성된 디렉토리로 이동
        if (i < count - 1) {  // 마지막 디렉토리 전까지만 이동
            MovePath(tree, parts[i]);
        }
    }

    // 원래 위치로 돌아가기 위해 count-1번 상위 디렉토리로 이동
    for (int i = 0; i < count - 1; i++) {
        MovePath(tree, "..");
    }

    return 0;
}

int command_mkdir(int argc, char** argv, DTree* tree, UList* users) {
    if (argc < 2) {
        fprintf(stderr, "mkdir: missing operand\n");
        return -1;
    }

    int parents = 0;
    int mode_given = 0;
    int custom_mode = 0777;  //  기본 퍼미션: rwxrwxrwx
    int i;

    // 옵션 처리
    for (i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--parents") == 0) {
            parents = 1;
        } else if (strcmp(argv[i], "-m") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "mkdir: option requires an argument -- 'm'\n");
                return -1;
            }
            custom_mode = strtol(argv[++i], NULL, 8);  // 8진수
            mode_given = 1;
        } else {
            fprintf(stderr, "mkdir: invalid option -- '%s'\n", argv[i]);
            return -1;
        }
    }

    // 디렉토리 생성
    for (; i < argc; i++) {
        const char* path = argv[i];

        if (parents) {
            if (create_parent_directories(tree, path, custom_mode, users) == -1) {
                fprintf(stderr, "mkdir: cannot create directory '%s'\n", path);
                return -1;
            }
        } else {
            if (ExistDir(tree, path, 'd')) {
                fprintf(stderr, "mkdir: cannot create directory '%s': Directory exists\n", path);
                return -1;
            }

            MakeDir(tree, (char*)path, 'd');

            // 퍼미션 적용
            TNode* newNode = ExistDir(tree, path, 'd');
            if (newNode) {
                int mode_to_set = mode_given ? custom_mode : 0777;
                for (int j = 0; j < 9; j++) {
                    newNode->permission[j] = (mode_to_set >> (8 - j)) & 1;
                }
                // 현재 사용자의 UID/GID 설정
                newNode->UID = users->current->UID;
                newNode->GID = users->current->GID;
            }
        }
    }

    return 0;
}
