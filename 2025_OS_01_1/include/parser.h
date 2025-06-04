#ifndef PARSER_H
#define PARSER_H

#include "osproject.h"

void ReplaceAlias(AliasTable* table, char* input);
void ParseAndExecute(DTree* tree, Stack* stack, UList* user, AliasTable* aliases, char* input);


#endif
