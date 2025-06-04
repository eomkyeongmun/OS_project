
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "osproject.h"

extern char *fileContentList[256];

TNode *FindFileInCurrentDir(TNode *current, const char *name)
{
    TNode *cursor = current->left;
    while (cursor)
    {
        if (strcmp(cursor->name, name) == 0 && cursor->type == 'f')
        {
            return cursor;
        }
        cursor = cursor->right;
    }
    return NULL;
}

void command_cat(DTree *tree, const char *filename)
{
    int line_numbering = 0;
    const char *targetFile = filename;

    // Option: "-n filename"
    if (strncmp(filename, "-n ", 3) == 0) {
        line_numbering = 1;
        targetFile = filename + 3;
        while (*targetFile == ' ') targetFile++;
    }

    // Write mode
    if (targetFile[0] == '>')
    {
        const char *actualName = targetFile + 1;
        while (*actualName == ' ')
            actualName++;

        printf("Enter content for '%s' (End with Ctrl+Z):\n", actualName);

        TNode *file = FindFileInCurrentDir(tree->current, actualName);
        if (!file)
        {
            file = (TNode *)calloc(1, sizeof(TNode));
            strncpy(file->name, actualName, MAXN - 1);
            file->name[MAXN - 1] = '\0';
            file->type = 'f';
            file->Parent = tree->current;

            for (int j = 0; j < 9; j++)
                file->permission[j] = 1;

            file->right = tree->current->left;
            tree->current->left = file;

            int idx = -1;
            for (int i = 0; i < 256; i++)
            {
                if (fileContentList[i] == NULL)
                {
                    idx = i;
                    break;
                }
            }
            if (idx == -1)
            {
                printf("Error: No space available to save content.\n");
                free(file);
                return;
            }
            file->contentIndex = idx;
        }

        char buffer[4096] = {0};
        char line[256];
        size_t total = 0;

        while (fgets(line, sizeof(line), stdin))
        {
            strncat(buffer, line, sizeof(buffer) - strlen(buffer) - 1);
            total += strlen(line);
        }

        if (feof(stdin)) clearerr(stdin);

        if (file->contentIndex != -1 && fileContentList[file->contentIndex])
            free(fileContentList[file->contentIndex]);
        
        fileContentList[file->contentIndex] = strdup(buffer);
        file->SIZE = (int)total; // 실제 파일 크기를 SIZE에 저장

        update_node_time(file); // 파일 생성/수정 시 시간 업데이트

        printf("Input complete. %zu bytes written.\n", total);
        return;
    }

    // Read mode
    TNode *file = FindFileInCurrentDir(tree->current, targetFile);
    if (!file || file->type != 'f')
    {
        printf("cat: file '%s' not found.\n", targetFile);
        return;
    }

    // Permission check
    if (IsPermission(file, 'r') != 0) {
        printf("cat: '%s': Permission denied.\n", file->name);
        return;
    }

    printf("Content of file (%s):\n", file->name);
    if (file->SIZE >= 0 && fileContentList[file->contentIndex])
    {
        if (line_numbering)
        {
            char *content = strdup(fileContentList[file->contentIndex]);
            char *line = strtok(content, "\n");
            int line_num = 1;
            while (line)
            {
                printf("%4d  %s\n", line_num++, line);
                line = strtok(NULL, "\n");
            }
            free(content);
        }
        else
        {
            printf("%s", fileContentList[file->contentIndex]);
        }
    }
    else
    {
        printf("(No content)\n");
    }
}