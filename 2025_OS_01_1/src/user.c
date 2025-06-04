#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "osproject.h"

// 새로운 사용자 노드 생성
UNode* create_user(const char* username, int uid, int gid) {
    UNode* new_user = (UNode*)malloc(sizeof(UNode));
    if (!new_user) return NULL;

    strncpy(new_user->name, username, sizeof(new_user->name) - 1);
    new_user->name[sizeof(new_user->name) - 1] = '\0';
    strcpy(new_user->dir, "/");
    new_user->UID = uid;
    new_user->GID = gid;
    new_user->linknode = NULL;
    return new_user;
}

// useradd 명령어 구현
int command_useradd(UList* users, int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: useradd <username>\n");
        return -1;
    }

    const char* username = argv[1];
    
    // 이미 존재하는 사용자인지 확인
    UNode* current = users->head;
    while (current) {
        if (strcmp(current->name, username) == 0) {
            fprintf(stderr, "useradd: user '%s' already exists\n", username);
            return -1;
        }
        current = current->linknode;
    }

    // 새로운 UID와 GID 생성
    int new_uid = (users->tUID < 1000) ? 1000 : users->tUID + 1;
    int new_gid = (users->tGID < 1000) ? 1000 : users->tGID + 1;

    // 새 사용자 생성
    UNode* new_user = create_user(username, new_uid, new_gid);
    if (!new_user) {
        fprintf(stderr, "useradd: failed to create user\n");
        return -1;
    }

    // 리스트에 추가
    if (!users->head) {
        users->head = users->tail = new_user;
    } else {
        users->tail->linknode = new_user;
        users->tail = new_user;
    }

    users->tUID = new_uid;
    users->tGID = new_gid;

    printf("User '%s' created with UID=%d, GID=%d\n", username, new_uid, new_gid);
    return 0;
}

// userdel 명령어 구현
int command_userdel(UList* users, int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: userdel <username>\n");
        return -1;
    }

    const char* username = argv[1];
    
    // 현재 로그인된 사용자는 삭제 불가
    if (users->current && strcmp(users->current->name, username) == 0) {
        fprintf(stderr, "userdel: cannot delete current user\n");
        return -1;
    }

    UNode* prev = NULL;
    UNode* current = users->head;

    while (current) {
        if (strcmp(current->name, username) == 0) {
            if (prev) {
                prev->linknode = current->linknode;
                if (current == users->tail) {
                    users->tail = prev;
                }
            } else {
                users->head = current->linknode;
                if (current == users->tail) {
                    users->tail = NULL;
                }
            }
            free(current);
            printf("User '%s' deleted\n", username);
            return 0;
        }
        prev = current;
        current = current->linknode;
    }

    fprintf(stderr, "userdel: user '%s' does not exist\n", username);
    return -1;
}

// userlist 명령어 구현
int command_userlist(UList* users) {
    printf("User List:\n");
    printf("USERNAME        UID  GID\n");
    printf("--------        ---  ---\n");

    UNode* current = users->head;
    while (current) {
        printf("%-14s %3d  %3d\n", 
            current->name, 
            current->UID, 
            current->GID);
        current = current->linknode;
    }
    return 0;
} 