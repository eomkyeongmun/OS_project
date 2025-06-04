#ifndef STACK_H
#define STACK_H

#include "osproject.h"

#define STACK_MAX 128

void Push(Stack *s, const char *str);
char *Pop(Stack *s);
int IsEmpty(const Stack *s);

#endif
