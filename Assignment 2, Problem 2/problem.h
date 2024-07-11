/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Header for module which contains Problem 2-related 
        data structures and functions.
*/
#include <stdio.h>
#include "prefixTree.h"

// For the indices of the cell minus by one to represent the directions of adjacent cells 
#define DIRECTIONS { \
    {1, 1, 1}, \
    {1, 0, 1}, \
    {1, 1, 1}  \
}

struct problem;
struct solution;

typedef struct {
    char character;
    int directions[3][3];
    int marked;
}Cell;

/*
    Function to compare two strings lexicographically first then by length
*/
int compareStrings(const void *a, const void *b);

/*
    Function to compare two chars by value
*/
int compareChars(const void *a, const void *b);

/*
    Finds word within board using DFS
*/
void searchWord(struct prefixTree *root, struct problem *p, struct solution *s, int x, int y);

/*
    Mark Cells to prevent over/duplicate disabling for adjacent cells
*/
void markCell(Cell **matrix, int dimension, int x, int y, int dirX, int dirY);

/*
    Finds next possible letters for word using DFS
*/
void searchNextChars(struct prefixTree *current, struct problem *p, struct solution *s, int x, int y);

/*
    Finds word within board using DFS whilst excluding words containing duplicateletters
*/
void searchWordOptimised(struct prefixTree *current, struct solution *s, Cell **matrix, int dimension, int x, int y);

/* 
    Reads the given dictionary file into a set of words
        and the given board file into a 1-D and 2-D representation.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile);

/* 
    Reads the given dictionary file into a set of words,
        the given board file into a 1-D and 2-D representation and
        the given partial string file as a partial string.
*/
struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
    FILE *partialStringFile);

/*
    Same as Problem A, but part is set for Part D.
*/
struct problem *readProblemD(FILE *dictFile, FILE *boardFile);

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p);

/*
    Solves the given problem according to Part B's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemB(struct problem *p);

/*
    Solves the given problem according to Part D's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemD(struct problem *p);

/*
    Outputs the given solution to the given file.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName);

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem);

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem);



