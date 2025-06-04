#include "command.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void ReplaceAlias(AliasTable* table, char* input) {
    char original[256];
    strcpy(original, input);

    char* token = strtok(original, " ");
    if (!token) return;

    for (int i = 0; i < table->count; i++) {
        if (strcmp(token, table->list[i].name) == 0) {
            const char* remaining = input + strlen(table->list[i].name);
            while (*remaining == ' ') remaining++;

            char replaced[512];
            if (*remaining) {
                snprintf(replaced, sizeof(replaced), "%s %s", table->list[i].command, remaining);
            } else {
                snprintf(replaced, sizeof(replaced), "%s", table->list[i].command);
            }

            strcpy(input, replaced);
            return;
        }
    }
}

void ParseAndExecute(DTree* tree, Stack* stack, UList* users, AliasTable* aliases, char* input) {
    ReplaceAlias(aliases, input);

    char* argv[10];
    int argc = 0;
    char* token = strtok(input, " ");
    while (token && argc < 10) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0) return;

    if (strcmp(argv[0], "ls") == 0) {
        command_ls(tree, users, argc, argv);
    } else if (strcmp(argv[0], "cd") == 0) {
        command_cd(tree, users, (argc > 1) ? argv[1] : NULL);
    } else if (strcmp(argv[0], "useradd") == 0) {
        command_useradd(users, argc, argv);
    } else if (strcmp(argv[0], "userdel") == 0) {
        command_userdel(users, argc, argv);
    } else if (strcmp(argv[0], "userlist") == 0) {
        command_userlist(users);
    } else if (strcmp(argv[0], "cat") == 0) {
        if (argc == 2 && strncmp(argv[1], ">", 1) == 0) {
            command_cat(tree, argv[1]);
        } else if (argc == 3 && strcmp(argv[1], ">") == 0) {
            char filenameWithArrow[128];
            snprintf(filenameWithArrow, sizeof(filenameWithArrow), ">%s", argv[2]);
            command_cat(tree, filenameWithArrow);
        } else if (argc == 2) {
            command_cat(tree, argv[1]);
        } else if (argc == 3 && strcmp(argv[1], "-n") == 0) {
            char temp[128];
            snprintf(temp, sizeof(temp), "-n %s", argv[2]);
            command_cat(tree, temp);
        } else {
            printf("Usage: cat <filename> or cat > <filename>\n");
        }
        return;
    } else if (strcmp(argv[0], "alias") == 0) {
        command_alias(aliases, argc, argv);
    } else if (strcmp(argv[0], "cp") == 0 && argc >= 3) {
        command_cp(tree, argv[1], argv[2]);
    } else if (strcmp(argv[0], "rm") == 0) {
        command_rm(tree, argc, argv);
    } else if (strcmp(argv[0], "diff") == 0 && argc >= 3) {
         command_diff(tree, argv[1], argv[2]);
    } else if (strcmp(argv[0], "find") == 0) {
        const char* name = NULL;
        char type = 0;

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-name") == 0 && i + 1 < argc) {
                name = argv[++i];
                if ((name[0] == '"' || name[0] == '\'') && name[strlen(name)-1] == name[0]) {
                    ((char*)name)[strlen(name)-1] = '\0';
                    name++;
                }
            } else if (strcmp(argv[i], "-type") == 0 && i + 1 < argc) {
                type = argv[++i][0];
            } else if (argv[i][0] != '-') {
                name = argv[i];
            }
        }

        command_find(tree, name ? name : "*", type);
    } else if (strcmp(argv[0], "grep") == 0) {
        command_grep(tree, argc, argv);
    } else if (strcmp(argv[0], "pwd") == 0) {
        command_pwd(tree, stack, (argc > 1) ? argv[1] : NULL);
    } else if (strcmp(argv[0], "mkdir") == 0) {
        command_mkdir(argc, argv, tree, users);
    } else if (strcmp(argv[0], "chown") == 0) {
        command_chown(argc, argv, tree, users);
    } else if (strcmp(argv[0], "unalias") == 0) {
        command_unalias(aliases, argc, argv);
    } else {
        printf("Unknown command: %s\n", argv[0]);
    }
}
