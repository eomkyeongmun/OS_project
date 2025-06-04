#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"

#define MAX_ENTRIES 256

// 특수 디렉토리 구조체
typedef struct {
    TNode* node;
    const char* display_name;
} SpecialDir;

void print_permissions(int permission[9]) {
    const char* rwx = "rwx";
    for (int i = 0; i < 9; i++) {
        printf("%c", permission[i] ? rwx[i % 3] : '-');
    }
}

void print_long_format(TNode* node, const char* display_name, UList* users) {
    printf("%c", node->type);
    print_permissions(node->permission);
    
    const char* username = GetNameByUID(users, node->UID);
    const char* groupname = GetNameByGID(users, node->GID);
    
    printf(" %s %s %5d %02d-%02d %02d:%02d %s\n",
        username, groupname, node->SIZE,
        node->month, node->day, node->hour, node->minute,
        display_name ? display_name : node->name);
}

void print_short_format(TNode* node, const char* display_name) {
    const char* name = display_name ? display_name : node->name;
    if (strlen(name) < 8)
        printf("%s\t\t", name);
    else
        printf("%s\t", name);
}

// 정렬을 위한 구조체
typedef struct {
    TNode* node;
    const char* display_name;
} LSEntry;

// 정렬을 위한 비교 함수
int compare_entries(const void* a, const void* b) {
    const LSEntry* entry1 = (const LSEntry*)a;
    const LSEntry* entry2 = (const LSEntry*)b;
    const char* name1 = entry1->display_name ? entry1->display_name : entry1->node->name;
    const char* name2 = entry2->display_name ? entry2->display_name : entry2->node->name;
    return strcmp(name1, name2);
}

// 디렉토리 노드의 권한을 복사하는 함수
void copy_permissions(int dest[9], int src[9]) {
    for(int i = 0; i < 9; i++) {
        dest[i] = src[i];
    }
}

void command_ls(DTree* tree, UList* users, int argc, char** argv) {
    int show_all = 0, long_format = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) show_all = 1;
        else if (strcmp(argv[i], "-l") == 0) long_format = 1;
        else if (strcmp(argv[i], "-al") == 0 || strcmp(argv[i], "-la") == 0)
            show_all = long_format = 1;
    }

    LSEntry entries[MAX_ENTRIES];
    int entry_count = 0;

    if (show_all) {
        // 현재 디렉토리 (.)
        TNode* current_entry = (TNode*)malloc(sizeof(TNode));
        memcpy(current_entry, tree->current, sizeof(TNode));
        current_entry->type = 'd';
        copy_permissions(current_entry->permission, tree->current->permission);
        entries[entry_count].node = current_entry;
        entries[entry_count].display_name = ".";
        entry_count++;

        // 상위 디렉토리 (..)
        TNode* parent = tree->current->Parent ? tree->current->Parent : tree->root;
        TNode* parent_entry = (TNode*)malloc(sizeof(TNode));
        memcpy(parent_entry, parent, sizeof(TNode));
        parent_entry->type = 'd';
        copy_permissions(parent_entry->permission, parent->permission);
        entries[entry_count].node = parent_entry;
        entries[entry_count].display_name = "..";
        entry_count++;
    }

    // 현재 디렉토리 내용 수집
    TNode* node = tree->current->left;
    while (node && entry_count < MAX_ENTRIES) {
        if (!show_all && node->name[0] == '.') {
            node = node->right;
            continue;
        }
        entries[entry_count].node = node;
        entries[entry_count].display_name = NULL;
        entry_count++;
        node = node->right;
    }

    // 알파벳 순으로 정렬
    qsort(entries, entry_count, sizeof(LSEntry), compare_entries);

    // 정렬된 순서로 출력
    int count = 0;
    for (int i = 0; i < entry_count; i++) {
        if (long_format) {
            print_long_format(entries[i].node, entries[i].display_name, users);
        } else {
            print_short_format(entries[i].node, entries[i].display_name);
            if (++count % 5 == 0) printf("\n");
        }
    }

    if (!long_format && count % 5 != 0) printf("\n");

    // 메모리 해제
    if (show_all) {
        free(entries[0].node);  // . 디렉토리
        free(entries[1].node);  // .. 디렉토리
    }
}
