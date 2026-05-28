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
    if (head == NULL) {
        new->index = 1;
        head = new;
    }
    else {
        Node *cur = head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        new->index = cur->index + 1;
        cur->next = new;
    }
    return head;
}


void print_lst(Node *head) {
    Node *cur = head;
    int count = 0;
    printf("\n");
    while (cur != NULL) {
        printf("%d: %s %d\n", cur->pid, cur->name, cur->index);
        cur = cur->next;
        count++;
    }
    printf("Total Background Jobs: %d\n", count);
}


Node* remove_by_pid(Node *head, __pid_t pid) {
    Node *cur = head;
    if (head->pid == pid) {
        free(head);
        head = cur->next;
    }
    while (cur->next != NULL) {
        if (cur->next->pid == pid) {
            Node *toRemove = cur->next;
            cur->next = toRemove->next;
            free(toRemove);
        }
        cur = cur->next;
    }
    return head;
}
