#include "osproject.h"
#include "command.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    TNode* node;
    char   target[MAXN];
    char   type;
} FindArgs;

static void clean_slashes(char* path) {
    char temp[MAXD] = {0};
    int  j = 0;
    for (int i = 0; path[i]; ++i) {
        if (path[i] == '/' && path[i + 1] == '/') continue;
        temp[j++] = path[i];
    }
    temp[j] = '\0';
    strncpy(path, temp, MAXD - 1);
}

static int wildcard_match(const char* pattern, const char* str) {
    while (*pattern && *str) {
        if (*pattern == '*') {
            pattern++;
            if (!*pattern) return 1;
            while (*str) {
                if (wildcard_match(pattern, str)) return 1;
                str++;
            }
            return 0;
        } else if (*pattern == *str || *pattern == '?') {
            pattern++;
            str++;
        } else {
            return 0;
        }
    }
    if (*pattern == '*' && !*(pattern + 1)) return 1;
    return (!*pattern && !*str);
}

static void build_full_path(TNode* node, char* buf) {
    if (!node || !buf) return;
    if (!node->Parent) {
        strcpy(buf, "/");
        return;
    }
    char temp[MAXD] = "";
    build_full_path(node->Parent, temp);
    if (strcmp(temp, "/") == 0) {
        snprintf(buf, MAXD, "/%s", node->name);
    } else {
        snprintf(buf, MAXD, "%s/%s", temp, node->name);
    }
}

static void print_found(const char* fullPath, char type) {
    char cleaned[MAXD];
    strncpy(cleaned, fullPath, MAXD - 1);
    cleaned[MAXD - 1] = '\0';
    clean_slashes(cleaned);

    const char* typeStr = (type == 'd') ? "Directory" :
                          (type == 'f') ? "File"      : "Unknown";
    printf("%s  [Type: %s]\n", cleaned, typeStr);
}

static void find_recursive(FindArgs* args) {
    TNode* cur = args->node;
    while (cur) {
        char curPath[MAXD];
        build_full_path(cur, curPath);
        clean_slashes(curPath);

        int nameMatch = 0;
        if (strcmp(args->target, "*") == 0) {
            nameMatch = 1;
        } else if (strchr(args->target, '*') || strchr(args->target, '?')) {
            nameMatch = wildcard_match(args->target, cur->name);
        } else {
            nameMatch = (strcmp(args->target, cur->name) == 0);
        }

        int typeMatch = 0;
        if (args->type == 0) typeMatch = 1;
        else if (args->type == 'd' && cur->type == 'd') typeMatch = 1;
        else if (args->type == 'f' && cur->type == 'f') typeMatch = 1;

        if (nameMatch && typeMatch) {
            print_found(curPath, cur->type);
        }

        if (cur->type == 'd' && cur->left) {
            FindArgs childArgs = *args;
            childArgs.node = cur->left;
            find_recursive(&childArgs);
        }

        cur = cur->right;
    }
}

static void find_subtree(FindArgs* args, TNode* start) {
    for (TNode* cur = start; cur; cur = cur->right) {
        char curPath[MAXD];
        build_full_path(cur, curPath);
        clean_slashes(curPath);

        int nameMatch = 0;
        if (strcmp(args->target, "*") == 0) {
            nameMatch = 1;
        } else if (strchr(args->target, '*') || strchr(args->target, '?')) {
            nameMatch = wildcard_match(args->target, cur->name);
        } else {
            nameMatch = (strcmp(args->target, cur->name) == 0);
        }

        int typeMatch = 0;
        if (args->type == 0) typeMatch = 1;
        else if (args->type == 'd' && cur->type == 'd') typeMatch = 1;
        else if (args->type == 'f' && cur->type == 'f') typeMatch = 1;

        if (nameMatch && typeMatch) {
            print_found(curPath, cur->type);
        }

        if (cur->type == 'd' && cur->left) {
            find_subtree(args, cur->left);
        }
    }
}

static TNode* FindNodeByName(TNode* node, const char* name) {
    if (!node) return NULL;
    if (strcmp(node->name, name) == 0) return node;

    TNode* found = FindNodeByName(node->left, name);
    if (found) return found;
    return FindNodeByName(node->right, name);
}

static TNode* FindNodeByPath(TNode* root, const char* path) {
    if (!root || !path || strlen(path) == 0) return NULL;

    char tempPath[MAXD];
    strncpy(tempPath, path, MAXD - 1);
    tempPath[MAXD - 1] = '\0';

    char* token = strtok(tempPath, "/");
    TNode* cur = root;

    while (token && cur) {
        TNode* child = cur->left;
        int found = 0;
        while (child) {
            if (strcmp(child->name, token) == 0) {
                cur = child;
                found = 1;
                break;
            }
            child = child->right;
        }
        if (!found) return NULL;
        token = strtok(NULL, "/");
    }
    return cur;
}

void command_find(DTree* tree, const char* raw, char type) {
    if (!tree || !tree->root || !tree->current) {
        printf("File system is not initialized.\n");
        return;
    }

    if (!raw || raw[0] == '\0') {
        FindArgs args0 = {0};
        args0.node = tree->current;
        strcpy(args0.target, "*");
        args0.type = type;
        if (tree->current->left) find_recursive(&args0);
        return;
    }

    if ((strcmp(raw, "-type f") == 0 && type == 'f') ||
        (strcmp(raw, "-type d") == 0 && type == 'd')) {
        FindArgs args1 = {0};
        args1.node = tree->root;
        strcpy(args1.target, "*");
        args1.type = type;
        if (tree->root->left) find_subtree(&args1, tree->root->left);
        return;
    }

    if (strchr(raw, '/')) {
        TNode* node = FindNodeByPath(tree->root, raw);
        if (!node) return;

        char fullPath[MAXD];
        build_full_path(node, fullPath);
        clean_slashes(fullPath);
        if (type == 0 || type == node->type) {
            print_found(fullPath, node->type);
        }

        if (node->type == 'd' && node->left) {
            FindArgs args2 = {0};
            args2.node = node;
            strcpy(args2.target, "*");
            args2.type = type;
            find_subtree(&args2, node->left);
        }
        return;
    }

    TNode* exactDir = FindNodeByName(tree->root, raw);
    if (exactDir && exactDir->type == 'd' &&
        !strchr(raw, '*') && !strchr(raw, '?') && raw[0] != '-') {
        char dirPath[MAXD];
        build_full_path(exactDir, dirPath);
        clean_slashes(dirPath);
        print_found(dirPath, exactDir->type);

        FindArgs args3 = {0};
        args3.node   = exactDir->left;
        strcpy(args3.target, "*");
        args3.type   = type;
        if (exactDir->left) find_subtree(&args3, exactDir->left);
        return;
    }

    FindArgs args4 = {0};
    args4.node = tree->current->left;
    strncpy(args4.target, raw, MAXN - 1);
    args4.target[MAXN - 1] = '\0';
    args4.type = type;
    if (tree->current->left) find_recursive(&args4);
}