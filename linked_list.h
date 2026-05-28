#include <stdio.h>

#ifndef HEADER
#define HEADER

typedef struct Node {
    char *name;
    __pid_t pid;
    int index;
    struct Node *next;
} Node;

Node* add(Node *head, __pid_t pid, char *name);

void print_lst(Node *head);

Node* remove_by_pid(Node *head, __pid_t pid);

#endif