#ifndef SYSTEM_H
#define SYSTEM_H

#include "osproject.h"

// 시스템 초기화 및 해제 함수들
Stack* StackInit();
void StackFree(Stack* s);

DTree* DLoad();
void FreeTree(DTree* tree);

UList* UserListLoad();
void UserListSave(UList* users);
void FreeUserList(UList* users);
void Login(UList* users, DTree* tree);
void PrintHeader(DTree* tree, Stack* stack, UList* users);

#endif
