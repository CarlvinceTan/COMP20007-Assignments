/*
    Skeleton written by Grady Fitzpatrick for 
    COMP20007 Assignment 1 2024 Semester 1
    
    Implementation details for module which contains doubly-linked list 
        specification data structures and functions.
    
    Implemented by Carlvince Tan
*/

#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct linkedListNode;

struct linkedList {
    struct linkedListNode *head;
    struct linkedListNode *tail;
};

struct linkedListNode {
    long double x;
    long double y;

    struct linkedListNode *next;
    struct linkedListNode *prev;
};

/* Print out each value in the list from the first value to the final value.
    Each value is printed with the format "(x, y) " where x is the x value
    set when inserting the item and y is the y value set when inserting 
    the item. */
void traverseRingForward(struct linkedList *list){
    struct linkedListNode *current = list->head;
    while (current != NULL) {
        printf("(%Lf, %Lf) ", current->x, current->y);
        current = current->next;
    }
}

/* Print out first the first item of the list, then print out each value in 
    the list from the final value to the second value.
    Each value is printed with the format "(x, y) " where x is the x value
    set when inserting the item and y is the y value set when inserting 
    the item. */
void traverseRingBackwards(struct linkedList *list){
    // Print the first item of the list
    printf("(%Lf, %Lf) ", list->head->x, list->head->y);
    // Print final to second value
    struct linkedListNode *current = list->tail;
    while (current->prev != NULL) {
        printf("(%Lf, %Lf) ", current->x, current->y);
        current = current->prev;
    }
}

/* Return a new empty list. */
struct linkedList *newList(){
    struct linkedList *list = (struct linkedList*)malloc(sizeof(struct linkedList));
    list->head = NULL;
    list->tail = NULL;
    
    return list;
}

/* Insert the given x, y pair at the head of the list */
void insertHead(struct linkedList *list, long double x, long double y){
    // Create new node
    struct linkedListNode* newHead = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
    assert(newHead != NULL);
    newHead->x = x;
    newHead->y = y;
    newHead->next = list->head;
    newHead->prev = NULL;

    if (list->head == NULL) {
        /* No existing node */
        list->head = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
        list->head->x = x;
        list->head->y = y;
        list->head->next = NULL;
        list->head->prev = NULL;

        list->tail = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
        list->tail->x = x;
        list->tail->y = y;
        list->tail->next = NULL;
        list->tail->prev = NULL;

    } else if (list->head->next == NULL && list->head->prev == NULL) {
        /* Only one node exists */
        list->head->prev = newHead;
        list->tail->prev = newHead;
        list->head = newHead;

    } else {
        /* List possess existing nodes */
        list->head->prev = newHead;
        list->head = newHead;

    }
}

/* Insert the given x, y pair at the tail of the list */
void insertTail(struct linkedList *list, long double x, long double y){
    // Create new node
    struct linkedListNode* newTail = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
    assert(newTail != NULL);
    newTail->x = x;
    newTail->y = y;
    newTail->next = NULL;
    newTail->prev = list->tail;

    if (list->tail == NULL) {
        /* No existing node */
        list->head = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
        list->head->x = x;
        list->head->y = y;
        list->head->next = NULL;
        list->head->prev = NULL;

        list->tail = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));
        list->tail->x = x;
        list->tail->y = y;
        list->tail->next = NULL;
        list->tail->prev = NULL;

    } else if (list->tail->next == NULL && list->tail->prev == NULL) {
        /* Only one node exists */
        list->tail->next = newTail;
        list->head->next = newTail;
        list->tail = newTail;

    } else {
        /* List possess existing nodes */
        list->tail->next = newTail;
        list->tail = newTail;
    }
}

/* Free all items in the given list. */
void freeList(struct linkedList *list){
    struct linkedListNode *current = list->head;
    struct linkedListNode *next = (struct linkedListNode*)malloc(sizeof(struct linkedListNode));

    // Free nodes
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(next);
}

