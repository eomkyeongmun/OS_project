
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "command.h"
#include "osproject.h"

extern char* fileContentList[256];

int strcasestr_custom(const char* haystack, const char* needle) {
    char lower_hay[1024], lower_need[256];
    size_t i;

    for (i = 0; haystack[i] && i < sizeof(lower_hay) - 1; i++)
        lower_hay[i] = tolower((unsigned char)haystack[i]);
    lower_hay[i] = '\0';

    for (i = 0; needle[i] && i < sizeof(lower_need) - 1; i++)
        lower_need[i] = tolower((unsigned char)needle[i]);
    lower_need[i] = '\0';

    return strstr(lower_hay, lower_need) != NULL;
}

void print_matches(const char* line, const char* word, int ignore_case) {
    const char* ptr = line;
    size_t len = strlen(word);
    while (*ptr) {
        if (ignore_case) {
            if (strncasecmp(ptr, word, len) == 0) {
                printf("%.*s\n", (int)len, ptr);
                ptr += len;
                continue;
            }
        } else {
            if (strncmp(ptr, word, len) == 0) {
                printf("%.*s\n", (int)len, ptr);
                ptr += len;
                continue;
            }
        }
        ptr++;
    }
}

void command_grep(DTree* tree, int argc, char** argv) {
    int show_line_number = 0;
    int ignore_case = 0;
    int invert_match = 0;
    int only_matching = 0;
    const char* word = NULL;
    const char* filename = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j]; j++) {
                if (argv[i][j] == 'n') show_line_number = 1;
                else if (argv[i][j] == 'i') ignore_case = 1;
                else if (argv[i][j] == 'v') invert_match = 1;
                else if (argv[i][j] == 'o') only_matching = 1;
                else {
                    printf("Unknown option: -%c\n", argv[i][j]);
                    return;
                }
            }
        } else if (!word) {
            word = argv[i];
        } else if (!filename) {
            filename = argv[i];
        } else {
            printf("Too many arguments.\n");
            printf("Usage: grep [-n] [-i] [-v] [-o] <pattern> <filename>\n");
            return;
        }
    }

    if (!word || !filename) {
        printf("Pattern or filename missing.\n");
        printf("Usage: grep [-n] [-i] [-v] [-o] <pattern> <filename>\n");
        return;
    }

    if (invert_match && only_matching) {
        printf("-o (only match) cannot be used with -v (invert match).\n");
        return;
    }

    TNode* file = ExistDir(tree, filename, 'f');
    if (!file || file->contentIndex < 0 || file->SIZE >= 256 || fileContentList[file->contentIndex] == NULL) {
        printf(" Cannot open file: %s\n", filename);
        return;
    }

    const char* content = fileContentList[file->contentIndex];
    char buffer[1024];
    strncpy(buffer, content, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    char* line = strtok(buffer, "\n");
    int line_num = 1;
    while (line) {
        int match = ignore_case
            ? strcasestr_custom(line, word)
            : strstr(line, word) != NULL;

        if (invert_match)
            match = !match;

        if (match) {
            if (only_matching) {
                print_matches(line, word, ignore_case);
            } else {
                if (show_line_number)
                    printf("%d: %s\n", line_num, line);
                else
                    printf("%s\n", line);
            }
        }
        line = strtok(NULL, "\n");
        line_num++;
    }
}