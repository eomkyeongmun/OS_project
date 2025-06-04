#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osproject.h"
#include "command.h"

extern char* fileContentList[256];
// ===== Utility Functions =====

// Extract directory portion from path
char* getDir(const char* path) {
    static char dir[MAXDIRECTORY];
    strncpy(dir, path, MAXDIRECTORY - 1);
    dir[MAXDIRECTORY - 1] = '\0';
    
    char* lastSlash = strrchr(dir, '/');
    if (lastSlash) {
        if (lastSlash == dir)
            *(lastSlash + 1) = '\0';  // root path "/"
        else
            *lastSlash = '\0';
    } else {
        dir[0] = '\0';
    }
    return dir;
}

// Search for a node with the given name and type in the current directory
TNode* ExistDir(DTree* tree, const char* name, char type) {
    for (TNode* t = tree->current->left; t != NULL; t = t->right) {
        if (strcmp(t->name, name) == 0 && t->type == type)
            return t;
    }
    return NULL;
}

// Remove a node from the current directory
void RemoveDir(DTree* tree, const char* name) {
    TNode* prev = NULL;
    TNode* cur = tree->current->left;

    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev)
                prev->right = cur->right;
            else
                tree->current->left = cur->right;
                if (cur->type == 'f' && cur->contentIndex >= 0 && cur->contentIndex < 256 && fileContentList[cur->contentIndex] != NULL) {
                free(fileContentList[cur->contentIndex]);
                fileContentList[cur->contentIndex] = NULL;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->right;
    }
}

// ===== Delete Command Handling =====

void delete_node(DTree* dTree, const char* path, int recursive, int force, int verbose) {
    TNode* currentN = dTree->current;
    TNode* target = NULL;
    char tmp[MAXDIRECTORY];
    char tmp2[MAXDIRECTORY];
    char tmp3[MAXDIRECTORY];
    char* str;
    int val;

    strncpy(tmp, path, MAXDIRECTORY - 1);
    tmp[MAXDIRECTORY - 1] = '\0';

    // When path contains a slash
    if (strchr(path, '/')) {
        strncpy(tmp2, getDir(path), MAXDIRECTORY - 1);
        tmp2[MAXDIRECTORY - 1] = '\0';

        char tmpPath[MAXDIRECTORY];
        strncpy(tmpPath, tmp2, MAXDIRECTORY - 1);
        tmpPath[MAXDIRECTORY - 1] = '\0';

        val = MovePath(dTree, tmpPath);
        if (val != 0) {
            if (!force)
                printf("rm: '%s': No such file or directory.\n", tmp2);
            return;
        }

        str = strtok(tmp, "/");
        while (str != NULL) {
            strncpy(tmp3, str, MAXNAME - 1);
            tmp3[MAXNAME - 1] = '\0';
            str = strtok(NULL, "/");
        }
    } else {
        strncpy(tmp3, path, MAXNAME - 1);
        tmp3[MAXNAME - 1] = '\0';
    }

    // Check if it's a directory
    target = ExistDir(dTree, tmp3, 'd');
    if (target && !recursive) {
        if (!force)
            printf("rm: cannot remove '%s': recursive option required.\n", tmp3);
        dTree->current = currentN;
        return;
    }
    if (!target) {
        target = ExistDir(dTree, tmp3, 'f');
    }

    if (!target) {
        if (!force)
            printf("rm: '%s': No such file or directory.\n", tmp3);
        dTree->current = currentN;
        return;
    }

    // Check write permission
    if (!force && IsPermission(target, 'w') != 0) {
        printf("rm: '%s': Permission denied.\n", tmp3);
        dTree->current = currentN;
        return;
    }

    // Recursive deletion
    if (target->type == 'd' && recursive) {
        TNode* parent_of_target_dir = dTree->current; // Save parent of target
        dTree->current = target;                      // Change current to the target directory

        while (target->left) {
            char child_name_buffer[MAXNAME];
            strncpy(child_name_buffer, target->left->name, MAXNAME - 1);
            child_name_buffer[MAXNAME - 1] = '\0';

            // Recursive call: dTree->current is target (the directory being deleted)
            delete_node(dTree, child_name_buffer, recursive, force, verbose);
            // After delete_node returns, target->left points to the next child or NULL
        }
        dTree->current = parent_of_target_dir; // Restore current to parent
    }

    if (verbose)
        printf("Deleted: %s\n", tmp3);

    RemoveDir(dTree, tmp3);
    dTree->current = currentN;  // Restore original current
}

// Entry point for rm command
void command_rm(DTree* tree, int argc, char** argv) {
    int recursive = 0, force = 0, verbose = 0;

    int i = 1;
    for (; i < argc && argv[i][0] == '-'; i++) {
        for (int j = 1; argv[i][j]; j++) {
            if (argv[i][j] == 'r') recursive = 1;
            else if (argv[i][j] == 'f') force = 1;
            else if (argv[i][j] == 'v') verbose = 1;
            else {
                printf("rm: unknown option -- '%c'\n", argv[i][j]);
                return;
            }
        }
    }

    for (; i < argc; i++) {
        delete_node(tree, argv[i], recursive, force, verbose);
    }
}