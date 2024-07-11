/*
    Written by William Sutherland for 
    COMP20007 Assignment 1 2023 Semester 1
    Modified by Grady Fitzpatrick and Carlvince Tan
    
    Implementation for module which contains
        stack-related data structures and 
        functions.
*/
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct stack *createStack(int item) {
    struct stack *s = (struct stack *) malloc(sizeof(struct stack));
    assert(s);
    s->item = item;
    s->below = NULL;
    return s;
}

int pop(struct stack **s) {
    if (!s || !*s)
        return -1;
        
    int item = (*s)->item;
    struct stack *b = (*s)->below;
    free(*s);
    *s = b;
    return item;
}

void push(struct stack **s, int item) {
    if (!s || !*s) {
        *s = createStack(item);
        return;
    }
    
    struct stack *top = createStack(item);
    top->below = *s;
    *s = top;
}

void freeStack(struct stack *s) {
    for (struct stack *curr = s; curr != NULL; s = curr) {
        curr = curr->below;
        free(s);
    }
}
