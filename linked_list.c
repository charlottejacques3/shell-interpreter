#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"


Node* add(Node *head, __pid_t pid, char *name) {

    // Create new node
    Node *new = malloc(sizeof(struct Node));
    if (new == NULL) {
        fprintf(stderr, "Error: malloc failed in linked list add()");
        exit(1);
    }
    new->next = NULL;
    new->pid = pid;
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);

    // Insert node
    if (head != NULL) {
        Node *cur = head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = new;
    }
    return new;
}


void print_lst(Node *head) {
    Node *cur = head;
    int i = 0;
    while (cur != NULL) {
        i++;
        printf("%d: %s %d\n", cur->pid, cur->name, i);
        cur = cur->next;
    }
    printf("Total Background Jobs: %d\n", i);
}

