#include "command.h"
#include "parser.h"
#include "system.h"  // system initialization functions

#include <stdio.h>
#include <string.h>

// Global variables (used as extern in diff.c, find.c)
DTree dtree;
char* fileContentList[256];

int main() {
    char cmd[256];

    
    DTree* tree = DLoad();                // Load directory tree
    Stack* stack = StackInit();           // Stack to track current path
    UList* users = UserListLoad();        // Load user list
    AliasTable aliasTable = {.count = 0}; // Initialize alias table

    // Login
    Login(users, tree);
    UserListSave(users);

    printf(" Welcome to the Mini OS Shell \n");
    printf("Type your command below (type 'exit' to quit):\n");

    while (1) {
        PrintHeader(tree, stack, users);

        if (!fgets(cmd, sizeof(cmd), stdin)) {
            clearerr(stdin);  // ⚠️ Handle Ctrl+D to continue input
            printf("\n❗ Empty input. Please continue.\n");
            continue;  // Don't terminate the program
        }

        cmd[strcspn(cmd, "\n")] = '\0';  // Remove newline character

        if (strlen(cmd) == 0) continue;
        if (strcmp(cmd, "exit") == 0) break;

        ParseAndExecute(tree, stack, users, &aliasTable, cmd);
    }

    // Free resources
    StackFree(stack);
    FreeTree(tree);
    FreeUserList(users);

    return 0;
}