/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 1-related data structures and 
        functions.
    
    Sample solution implemented by Grady Fitzpatrick
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

#define LDINFINITY (LDBL_MAX / 2.0L)

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

void readSequence(FILE *seqFile, int *seqLen, long double **seq);

void readSequence(FILE *seqFile, int *seqLen, long double **seq){
    char *seqText = NULL;
    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&seqText, &allocated, '\0', seqFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int seqTextLength = strlen(seqText);

    int commaCount = 0;
    /* Count how many numbers are present. */
    for(int i = 0; i < seqTextLength; i++){
        if(seqText[i] == ','){
            commaCount++;
        }
    }
    long double *seqLocal = (long double *) malloc(sizeof(long double) * (commaCount + 1));
    assert(seqLocal);

    int seqAdded = 0;
    while(progress < seqTextLength){
        int nextProgress;
        /* Read each value into the sequence. */
        assert(sscanf(seqText + progress, "%Lf , %n", &seqLocal[seqAdded], &nextProgress) == 1);
        assert(nextProgress > 0);
        progress += nextProgress;
        seqAdded++;
    }
    assert(seqAdded == (commaCount + 1));
    *seq = seqLocal;
    *seqLen = seqAdded;
}

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *seqAFile, FILE *seqBFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    int seqALength = 0;
    long double *seqA = NULL;
    readSequence(seqAFile, &seqALength, &seqA);
    int seqBLength = 0;
    long double *seqB = NULL;
    readSequence(seqBFile, &seqBLength, &seqB);

    /* The length of the first sequence. */
    p->seqALength = seqALength;
    /* The first sequence. */
    p->sequenceA = seqA;

    /* The length of the second sequence. */
    p->seqBLength = seqBLength;
    /* The second sequence. */
    p->sequenceB = seqB;

    /* For Part D & F only. */
    p->windowSize = -1;
    p->maximumPathLength = -1;

    p->part = PART_A;

    return p;
}

struct problem *readProblemD(FILE *seqAFile, FILE *seqBFile, int windowSize){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(seqAFile, seqBFile);

    p->part = PART_D;
    p->windowSize = windowSize;

    return p;
}

struct problem *readProblemF(FILE *seqAFile, FILE *seqBFile, 
    int maxPathLength){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(seqAFile, seqBFile);
    
    p->part = PART_F;
    p->maximumPathLength = maxPathLength;

    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    fprintf(outfileName, "%.2Lf\n", solution->optimalValue);
    switch(problem->part){
        case PART_A:
            assert(solution->matrix);
            for(int i = 1; i <= problem->seqALength; i++){
                for(int j = 1; j <= problem->seqBLength; j++){
                    if(solution->matrix[i][j] == LDINFINITY){
                        fprintf(outfileName, "    ");
                    } else {
                        fprintf(outfileName, "%.2Lf", solution->matrix[i][j]);
                    }
                    if(j < (problem->seqBLength)){
                        /* Intercalate with spaces. */
                        fprintf(outfileName, " ");
                    }
                }
                fprintf(outfileName, "\n");
            }
            break;
        case PART_D:
        case PART_F:
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        if(solution->matrix){
            for(int i = 0; i < problem->seqALength; i++){
                free(solution->matrix[i]);
            }
            free(solution->matrix);
        }
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->sequenceA){
            free(problem->sequenceA);
        }
        if(problem->sequenceB){
            free(problem->sequenceB);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    if(problem->part == PART_F){
        s->matrix = NULL;
    } else {
        s->matrix = (long double **) malloc(sizeof(long double *) * 
            (problem->seqALength + 1));
        assert(s->matrix);
        for(int i = 0; i < (problem->seqALength + 1); i++){
            s->matrix[i] = (long double *) malloc(sizeof(long double) * 
                (problem->seqBLength + 1));
            assert(s->matrix[i]);
            for(int j = 0; j < (problem->seqBLength + 1); j++){
                s->matrix[i][j] = 0;
            }
        }
    }

    s->optimalValue = -1;
    
    return s;
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);

    // Initialise length of sequences
    int n = p->seqALength;
    int m = p->seqBLength;

    // Create and initialise the DTW matrix with infinity
    for (int i = 1; i <= n; i++) {
        s->matrix[i][0] = LDINFINITY;
    }
    for (int i = 1; i <= m; i++) {
        s->matrix[0][i] = LDINFINITY;
    }
    s->matrix[0][0] = 0;
    
    // Populate the DTW matrix
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            // Calculate cost using the absolute difference between sequence points
            long double cost = fabsl(p->sequenceA[i - 1] - p->sequenceB[j - 1]);

            // Selects minimum of the 3 neighbouring cells
            long double min = fminl(s->matrix[i - 1][j - 1], fminl(s->matrix[i - 1][j], s->matrix[i][j - 1]));

            // Total cost
            s->matrix[i][j] = cost + min;
        }
    } 

    // The DTW distance is in the bottom-right corner of the matrix
    s->optimalValue = s->matrix[n][m];

    return s;
}

struct solution *solveProblemD(struct problem *p){
    struct solution *s = newSolution(p);

    // Initialise length of sequences
    int n = p->seqALength;
    int m = p->seqBLength;

    // Create and initialise the DTW matrix with infinity
    for (int i = 1; i <= n; i++) {
        s->matrix[i][0] = LDINFINITY;
    }
    for (int i = 1; i <= m; i++) {
        s->matrix[0][i] = LDINFINITY;
    }
    s->matrix[0][0] = 0;
    
    // Populate the DTW matrix
    for (int i = 1; i <= n; i++) {
        // Sets window bounds for each row
        int lower_bound = i - p->windowSize;
        int upper_bound = i + p->windowSize;
        if (lower_bound < 1) {
            lower_bound = 1;
        }
        if (upper_bound > m) {
            upper_bound = m;
        }
        
        for (int j = lower_bound; j <= upper_bound; j++) {
            // Calculate cost using the absolute difference between sequence points
            long double cost = fabsl(p->sequenceA[i - 1] - p->sequenceB[j - 1]);

            // Selects minimum of the 3 neighbouring cells if within boundary constraints
            long double min = s->matrix[i - 1][j - 1];
            if (j != lower_bound) {
                min = fminl(min, s->matrix[i][j - 1]);
            }
            if (j != upper_bound) {
                min = fminl(min, s->matrix[i - 1][j]);
            }

            // Total cost
            s->matrix[i][j] = cost + min;
        }
    } 

    // The DTW distance is in the bottom-right corner of the matrix
    s->optimalValue = s->matrix[n][m];

    return s;
}

struct solution *solveProblemF(struct problem *p){
    struct solution *s = newSolution(p);

    // Initialise length of sequences
    int n = p->seqALength;
    int m = p->seqBLength;
    int height = p->maximumPathLength;

    // Create 3D matrix with dimensions n x m x min(n, m, p->maximumPathLength)
    long double ***matrix = malloc((n + 1) * sizeof(long double **));
    for (int i = 0; i <= n; i++) {
        matrix[i] = malloc((m + 1) * sizeof(long double *));
        for (int j = 0; j <= m; j++) {
            matrix[i][j] = malloc((height + 1) * sizeof(long double));
        }
    }

    for (int k = 0; k <= height; k++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= m; j++) {
                matrix[i][j][k] = LDINFINITY;
            }
        }
    }
    matrix[0][0][0] = 0;

    for (int k = 1; k <= height; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (i + j > k) {
                    // Calculate cost using the absolute difference between sequence points
                    long double cost = fabsl(p->sequenceA[i - 1] - p->sequenceB[j - 1]);
                
                    // Selects neighbouring cells
                    long double min = fminl(matrix[i - 1][j - 1][k - 1], fminl(matrix[i - 1][j][k - 1], matrix[i][j - 1][k - 1]));

                    // Total cost
                    matrix[i][j][k] = cost + min;
                }
            }
        }
    }

    // Choose minimum DTW distances for all possible path lengths
    s->optimalValue = LDINFINITY;
    for (int k = 1; k <= height; k++) {
        if (matrix[n][m][k] < s->optimalValue) {
            s->optimalValue = matrix[n][m][k];
        }
    }

    // Free memory for the matrix
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            free(matrix[i][j]);  // Free memory for the third dimension
        }
        free(matrix[i]);  // Free memory for the second dimension
    }
    free(matrix);  // Free memory for the first dimension

    return s;
}

