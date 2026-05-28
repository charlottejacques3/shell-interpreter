#include <stdio.h>

#ifndef HEADER
#define HEADER

typedef struct Node {
    char *name;
    __pid_t pid;
    struct Node *next;
} Node;

Node* add(Node *head, __pid_t pid, char *name);

void print_lst(Node *head);

#endif