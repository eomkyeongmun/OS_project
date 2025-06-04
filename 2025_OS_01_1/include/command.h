#ifndef COMMAND_H
#define COMMAND_H
#include "osproject.h"

void command_ls(DTree* tree, UList* users, int argc, char** argv);
int command_cd(DTree*, UList* users, const char*);
// int command_cd(DTree *dTree, UList* users, const char *command, Stack* stack);
void command_cat(DTree *tree, const char *filename);
void ReplaceAlias(AliasTable* table, char* input);
void command_alias(AliasTable* table, int argc, char** argv);
void command_unalias(AliasTable* table, int argc, char** argv);
void command_rm(DTree* tree, int argc, char** argv);
void command_diff(DTree* tree, const char* name1, const char* name2);
void command_grep(DTree* tree, int argc, char** argv);
void command_find(DTree* tree, const char* raw, char type);
int command_cp(DTree *tree, char *src, char *dst);
int command_mkdir(int argc, char** argv, DTree* tree, UList* users);
int command_chown(int argc, char** argv, DTree* tree, UList* users);
int command_pwd(DTree* tree, Stack* stack, const char* arg);

int MovePath(DTree* tree, const char* path);
int Movecurrent(DTree* tree, const char* name);

TNode* ExistDir(DTree* tree, const char* name, char type);
int IsPermission(TNode* node, char mode);
void RemoveDir(DTree* tree, const char* name);
void UpdateUserDir(UList* users, DTree* tree);
void DeleteFile(DTree* tree, char* name);
void MakeDir(DTree* tree, char* name, char type);
void update_node_time(TNode* node);

int create_parent_directories(DTree* tree, const char* path, int mode, UList* users);


void PrintPath(DTree* tree, Stack* stack);
void Push(Stack* stack, const char* name);
int IsEmpty(Stack* stack);
char* Pop(Stack* stack);

// User management commands
int command_useradd(UList* users, int argc, char** argv);
int command_userdel(UList* users, int argc, char** argv);
int command_userlist(UList* users);

// User name conversion functions
const char* GetNameByUID(UList* users, int uid);
const char* GetNameByGID(UList* users, int gid);

#endif
