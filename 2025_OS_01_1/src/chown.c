#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "command.h"
#include "osproject.h"

//  UID 또는 GID를 사용자 이름으로 변환 (ls -l 용)
const char* GetNameByUID(UList* users, int uid) {
    UNode* cur = users->head;
    while (cur) {
        if (cur->UID == uid) return cur->name;
        cur = cur->linknode;
    }
    return "unknown";
}

const char* GetNameByGID(UList* users, int gid) {
    UNode* cur = users->head;
    while (cur) {
        if (cur->GID == gid) return cur->name;
        cur = cur->linknode;
    }
    return "unknown";
}

//  사용자명 → UID, 그룹명 → GID 변환
int GetUIDByName(UList* users, const char* name) {
    UNode* cur = users->head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur->UID;
        cur = cur->linknode;
    }
    return -1;
}

int GetGIDByName(UList* users, const char* name) {
    UNode* cur = users->head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur->GID;
        cur = cur->linknode;
    }
    return -1;
}

// 실제 TNode 변경 함수
void change_node_owner(TNode* node, int uid, int gid, int recursive) {
    node->UID = uid;
    if (gid != -1) node->GID = gid;

    if (recursive && node->type == 'd') {
        TNode* child = node->left;
        while (child) {
            change_node_owner(child, uid, gid, 1);
            child = child->right;
        }
    }
}

int command_chown(int argc, char** argv, DTree* tree, UList* users) {
    if (argc < 3) {
        fprintf(stderr, "Usage: chown [-R] owner[:group] target...\n");
        return -1;
    }

    int recursive = 0;
    int i = 1;

    if (strcmp(argv[1], "-R") == 0 || strcmp(argv[1], "--recursive") == 0) {
        recursive = 1;
        i++;
    }

    if (i >= argc) {
        fprintf(stderr, "chown: missing operand after option\n");
        return -1;
    }

    char* owner_spec = strdup(argv[i++]);
    char* colon = strchr(owner_spec, ':');
    char* user = NULL;
    char* group = NULL;
    int uid = -1, gid = -1;

    if (colon) {
        *colon = '\0';
        user = owner_spec;
        group = colon + 1;
    } else {
        user = owner_spec;
    }

    // 사용자 이름 또는 숫자 → UID
    if (isdigit(*user)) {
        uid = atoi(user);
    } else {
        // 사용자 찾기
        UNode* cur = users->head;
        while (cur) {
            if (strcmp(cur->name, user) == 0) {
                uid = cur->UID;
                break;
            }
            cur = cur->linknode;
        }
        if (uid == -1) {
            fprintf(stderr, "chown: invalid user '%s'\n", user);
            free(owner_spec);
            return -1;
        }
    }

    // 그룹 처리
    if (group) {
        if (isdigit(*group)) {
            gid = atoi(group);
        } else {
            // 그룹 찾기
            UNode* cur = users->head;
            while (cur) {
                if (strcmp(cur->name, group) == 0) {
                    gid = cur->GID;
                    break;
                }
                cur = cur->linknode;
            }
            if (gid == -1) {
                fprintf(stderr, "chown: invalid group '%s'\n", group);
                free(owner_spec);
                return -1;
            }
        }
    }

    // 파일들에 적용
    int result = 0;
    for (; i < argc; i++) {
        TNode* node = ExistDir(tree, argv[i], 'd');
        if (!node) node = ExistDir(tree, argv[i], 'f');
        if (!node) {
            fprintf(stderr, "chown: cannot access '%s': No such file or directory\n", argv[i]);
            result = -1;
            continue;
        }
        change_node_owner(node, uid, gid, recursive);
    }

    free(owner_spec);
    return result;
}
