#include <stdio.h>
#include <string.h>
#include "command.h"

void command_alias(AliasTable* table, int argc, char** argv) {
    if (argc == 1) {
        for (int i = 0; i < table->count; i++) {
            printf("alias %s='%s'\n", table->list[i].name, table->list[i].command);
        }
        return;
    }

    // Concatenate argv[1] ~ argv[argc-1] into one full string
    char combined[256] = {0};
    for (int i = 1; i < argc; i++) {
        strcat(combined, argv[i]);
        if (i != argc - 1) strcat(combined, " ");
    }

    char* equal = strchr(combined, '=');
    if (!equal) {
        printf("Invalid alias format. (ex: alias ll=\"ls -l\")\n");
        return;
    }

    int nameLen = equal - combined;
    char name[32] = {0};
    strncpy(name, combined, nameLen);

    char* commandStr = equal + 1;

    // Remove quotes if present
    if (commandStr[0] == '\'' || commandStr[0] == '"') commandStr++;
    size_t len = strlen(commandStr);
    if (len > 0 && (commandStr[len - 1] == '\'' || commandStr[len - 1] == '"')) {
        commandStr[len - 1] = '\0';
    }

    if (table->count < MAX_ALIAS) {
        strcpy(table->list[table->count].name, name);
        strcpy(table->list[table->count].command, commandStr);
        table->count++;
    } else {
        printf("Alias limit exceeded.\n");
    }
}

void command_unalias(AliasTable* table, int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: unalias <alias_name>\n");
        return;
    }

    const char* name = argv[1];
    int found = 0;

    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->list[i].name, name) == 0) {
            // Shift remaining entries forward
            for (int j = i; j < table->count - 1; j++) {
                table->list[j] = table->list[j + 1];
            }
            table->count--;
            found = 1;
            printf("Alias '%s' removed.\n", name);
            break;
        }
    }

    if (!found) {
        printf("Alias '%s' not found.\n", name);
    }
}