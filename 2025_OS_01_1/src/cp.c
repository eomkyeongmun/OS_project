#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"
#include "osproject.h"

extern char* fileContentList[256];

#define MAXPATH 1024

int command_cp(DTree* tree, char* src, char* dst)
{
    TNode* original_current_dir = tree->current;
    TNode* srcFile = ExistDir(tree, src, 'f');
    if (!srcFile) {
        printf("Source file '%s' does not exist in current directory.\n", src);
        return -1;
    }
    if (srcFile->contentIndex < 0 || srcFile->contentIndex >= 256 || fileContentList[srcFile->contentIndex] == NULL) {
        printf("Copy failed: source file has no content.\n");
        return -1;
    }
    char dstPathCopy[MAXPATH];
    strncpy(dstPathCopy, dst, MAXPATH - 1);
    dstPathCopy[MAXPATH - 1] = '\0';
    TNode* destDir = NULL;
    char newFileName[MAXN];
    char tempPathForMove[MAXPATH];
    strncpy(tempPathForMove, dstPathCopy, MAXPATH - 1);
    tempPathForMove[MAXPATH - 1] = '\0';

    if (MovePath(tree, tempPathForMove) == 0) {
        destDir = tree->current;
        const char* base = strrchr(src, '/');
        strcpy(newFileName, base ? base + 1 : src);
    } else {
        
        tree->current = original_current_dir; 
        char* lastSlash = strrchr(dstPathCopy, '/');
        if (lastSlash) {
            *lastSlash = '\0';
            strcpy(newFileName, lastSlash + 1);

            if (MovePath(tree, dstPathCopy) == 0) {
                destDir = tree->current;
            } else {
                printf("Copy failed: target directory '%s' does not exist.\n", dstPathCopy);
                tree->current = original_current_dir;
                return -1;
            }
        } else {
            destDir = tree->current;
            strcpy(newFileName, dstPathCopy); 
        }
    }

    if (!destDir) {
        printf("Copy failed: target directory could not be determined.\n");
        tree->current = original_current_dir;
        return -1;
    }

    if (destDir == srcFile->Parent && strcmp(srcFile->name, newFileName) == 0) {
        printf("Source and destination are the same.\n");
        tree->current = original_current_dir;
        return -1;
    }

    TNode *prev = NULL, *cur = destDir->left;
    while (cur) {
        if (cur->type == 'f' && strcmp(cur->name, newFileName) == 0) {
            if (prev)
                prev->right = cur->right;
            else
                destDir->left = cur->right;

            if (cur->contentIndex != -1 && fileContentList[cur->contentIndex] != NULL) {
                free(fileContentList[cur->contentIndex]);
                fileContentList[cur->contentIndex] = NULL;
            }
            free(cur);
            break;
        }
        prev = cur;
        cur  = cur->right;
    }
    TNode* newFile = (TNode*)calloc(1, sizeof(TNode));
    strncpy(newFile->name, newFileName, MAXN - 1);
    newFile->name[MAXN - 1] = '\0';
    newFile->type = 'f';
    newFile->Parent = destDir;

    for (int i = 0; i < 9; i++) {
        newFile->permission[i] = srcFile->permission[i];
    }
    newFile->UID = srcFile->UID;
    newFile->GID = srcFile->GID;

    update_node_time(newFile); 

    int newIdx = -1;
    for (int i = 0; i < 256; i++) {
        if (fileContentList[i] == NULL) {
            newIdx = i;
            break;
        }
    }
    if (newIdx == -1) {
        printf("Copy failed: no space available.\n");
        free(newFile);
        tree->current = original_current_dir;
        return -1;
    }
    newFile->contentIndex = newIdx;
    newFile->SIZE = srcFile->SIZE;

    fileContentList[newIdx] = strdup(fileContentList[srcFile->contentIndex]);

    newFile->right = destDir->left;
    destDir->left = newFile;

    printf("Copy complete: %s -> ", src);
    if (destDir == tree->root) {
        printf("/"); // 루트 디렉토리
    } else if (destDir == original_current_dir) {
        printf("./"); // 현재 디렉토리
    } else {
        printf("%s/", destDir->name);
    }
    printf("%s\n", newFileName);
    tree->current = original_current_dir; // 모든 작업 후 원래 디렉토리로 복귀
    return 0;
}