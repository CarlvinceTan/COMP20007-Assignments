/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 1 2024 Semester 1
    
    Header for module which contains convex hull 
        specification data structures and functions.
*/

#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include "linkedList.h"
#include "problem.h"

struct solution;

struct solution {
    struct linkedList *convexHull;
    int operationCount;
};

enum orientationResult {
    COLLINEAR = 0,
    CLOCKWISE = 1,
    COUNTERCLOCKWISE = 2
};

/* Finds the orientation between the three given points - calculates the angle between 
    the Middle-First vector and Middle-Final vector - if the Middle-First vector is zero-length,
    the gradient of the Middle-Final vector is instead used. */
enum orientationResult orientation(struct problem *p, int idxFirst, int idxMiddle, int idxFinal);

/* Part A - perform Jarvis' March to construct a convex
    hull for the given problem. */
struct solution *jarvisMarch(struct problem *p);

/* Part B - perform Graham's Scan to construct a convex
    hull for the given problem. */
struct solution *grahamScan(struct problem *p);

/* Frees solution structure and all contained information. */
void freeSolution(struct solution *s);

#endif


