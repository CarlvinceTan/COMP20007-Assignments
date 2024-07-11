/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Implementation for module which contains Prefix Trie 
        data structures and functions.
*/
#include "prefixTree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#include <error.h>
#include <string.h>

struct prefixTree *newPrefixTree(){
    /* Structure of the prefix tree includes a dummy node. */
    struct prefixTree *retTree = (struct prefixTree*)malloc(sizeof(struct prefixTree));
    retTree->mark = UNMARKED;
    retTree->countChildren = 0;
    retTree->string = NULL;
    for (int i = 0; i < CHILD_COUNT; i++) {
        retTree->children[i] = NULL;
    }
    return retTree;
}

struct prefixTree *addWordToTree(struct prefixTree *pt, char *word){
    struct prefixTree *current = pt;
    while(*word) {
        if (current->children[(unsigned char) *word] == NULL) {
            current->children[(unsigned char) *word] = newPrefixTree();
            if (current->string == NULL) {
                current->children[(unsigned char) *word]->string = (char *) malloc(2 * sizeof(char));
                current->children[(unsigned char) *word]->string[0] = *word;                                   
                current->children[(unsigned char) *word]->string[1] = '\0';
            } else {
                current->children[(unsigned char) *word]->string = (char *)malloc((strlen(current->string) + 2) * sizeof(char));
                strcpy(current->children[(unsigned char) *word]->string, current->string);
                current->children[(unsigned char) *word]->string[strlen(current->string)] = *word;                                      
                current->children[(unsigned char) *word]->string[strlen(current->string) + 1] = '\0';
            }
        }
        current->children[(unsigned char) *word]->countChildren++;
        current = current->children[(unsigned char) *word];
        word++;
    }
    current->children[(unsigned char) '\0'] = newPrefixTree();
    current->children[(unsigned char) '\0']->string = (char *)malloc((strlen(current->string) + 1) * sizeof(char));
    strcpy(current->children[(unsigned char) '\0']->string, current->string);
    return pt;
}


