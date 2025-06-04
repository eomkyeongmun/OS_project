#include <stdio.h>
#include <string.h>
#include "command.h"

int command_pwd(DTree *dTree, Stack *dStack, const char *opt)
{
    if (opt && strcmp(opt, "--help") == 0) {
        puts("Guide : pwd");
        puts("Print the name of the current working directory.");
        return 0;
    }
    if (opt && opt[0] == '-') {
        printf("pwd: invalid option -- '%s'\n", opt);
        puts("Try 'pwd --help' for more information.");
        return -1;
    }

    PrintPath(dTree, dStack);
    return 0;
}

void PrintPath(DTree *dTree, Stack *dStack)
{
    TNode *cursor = dTree->current;

    if (cursor == dTree->root) {
        puts("/");
        return;
    }

    while (cursor && cursor->Parent) {
        Push(dStack, cursor->name);       
        cursor = cursor->Parent;
    }

    while (!IsEmpty(dStack)) {
        printf("/%s", Pop(dStack));
    }
    puts("");
    return;
}
