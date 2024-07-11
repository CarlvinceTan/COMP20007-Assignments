/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 1 2024 Semester 1
    
    Modified by Carlvince Tan

*/
#include "convexHull.h"
#include "problem.h"
#include "linkedList.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    struct problem *problem = readProblem(stdin, PART_B);

    struct solution *solution = grahamScan(problem);

    /* Print points in clockwise order. */
    traverseRingBackwards(solution->convexHull);
    
    printf("\n");

    /* Print points in counterclockwise order. */
    traverseRingForward(solution->convexHull);

    printf("\n");

    freeSolution(solution);
    freeProblem(problem);

    return EXIT_SUCCESS;
}

