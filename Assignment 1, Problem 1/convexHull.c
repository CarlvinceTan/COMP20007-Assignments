/*
    Skeleton written by Grady Fitzpatrick for 
    COMP20007 Assignment 1 2024 Semester 1
    
    Header for module which contains convex hull 
        specification data structures and functions.

    Implemented by Carlvince Tan
*/
#include "linkedList.h"
#include "problem.h"
#include "convexHull.h"
#include "stack.h"
#include "float.h"
#include "pq.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Finds the orientation between the three given points - calculates the angle between 
    the Middle-First vector and Middle-Final vector - if the Middle-First vector is zero-length,
    the gradient of the Middle-Final vector is instead used. */
enum orientationResult orientation(struct problem *p, int idxFirst, int idxMiddle, int idxFinal){
    assert(idxFirst >= 0 && idxFirst < p->numPoints);
    assert(idxMiddle >= 0 && idxMiddle < p->numPoints);
    assert(idxFinal >= 0 && idxFinal < p->numPoints);

    /* Use cross-product to calculate turn direction. */
    long double p0x = p->pointsX[idxFirst];
    long double p0y = p->pointsY[idxFirst];

    long double p1x = p->pointsX[idxMiddle];
    long double p1y = p->pointsY[idxMiddle];

    long double p2x = p->pointsX[idxFinal];
    long double p2y = p->pointsY[idxFinal];

    /* Cross product of vectors P1P0 & P1P2 */
    long double crossProduct = (p0x - p1x) * (p2y - p1y) - (p0y - p1y) * (p2x - p1x);

    if(crossProduct == 0){
        if(idxFirst == idxMiddle){
            /* Special case where we are only looking for positive slope of P1P2. */
            if(p2x == p1x){
                /* Special case: dx = 0, vertical */
                if(p2y < p1y){
                    /* Directly upwards */
                    return COUNTERCLOCKWISE;
                } else if(p2y == p1y){
                    /* Same point. */
                    return COLLINEAR;
                } else {
                    return CLOCKWISE;
                }
            }
            long double m = (p2y - p1y) / (p2x - p1x);
            if(m >= 0){
                return COUNTERCLOCKWISE;
            } else {
                return CLOCKWISE;
            }
            
        }
        return COLLINEAR;
    } else if(crossProduct > 0){
        return CLOCKWISE;
    } else {
        return COUNTERCLOCKWISE;
    }
}

struct solution *jarvisMarch(struct problem *p){
    /* Part A - perform Jarvis' March to construct a convex
    hull for the given problem. */

    // Initialize solution with empty convex hull points list
    struct solution *sol = malloc(sizeof(struct solution));

    // Initialize empty linkedlist
    sol->convexHull = newList();
    sol->operationCount = 0;
    
    // Ensure there are at least 3 points
    if (p->numPoints < 3) {
        return sol;
    }

    // Find the leftmost point among the given points
    long double leftmost_pointX = LDBL_MAX;
    long double leftmost_pointY = 0;
    int leftmost_point;
    for (int i = 0; i < p->numPoints; i++) {
        if (p->pointsX[i] < leftmost_pointX) {
            leftmost_pointX = p->pointsX[i];
            leftmost_pointY = p->pointsY[i];
            leftmost_point = i;
        } else if (p->pointsX[i] == leftmost_pointX && p->pointsY[i] < leftmost_pointY) {
            leftmost_pointX = p->pointsX[i];
            leftmost_pointY = p->pointsY[i];
            leftmost_point = i;
        }
    }

    // Setup current point and next point
    int current = leftmost_point;
    int next_point;

    // Commence Jarvis March
    while (1) {
        // Add current point to the convex hull
        insertTail(sol->convexHull, p->pointsX[current], p->pointsY[current]);

        // Find the next point such that it forms a counterclockwise turn
        // with the current point and any other point in the set
        next_point = 0;
        
        for (int i = 0; i < p->numPoints; i++) {
            if (next_point == current || orientation(p, next_point, current, i) == 2) {
                next_point = i;
            }
        }
        // Set 'nextPoint' as the current point for the next iteration
        current = next_point;
        
        if (current == leftmost_point) {
            break;
        }
    }

    // Return the list of points in the convex hull
    return sol;
}

struct solution *grahamScan(struct problem *p){
    /* Part B - perform Graham's Scan to construct a convex
    hull for the given problem. */

    // Initialize solution with empty convex hull points list
    struct solution *sol = malloc(sizeof(struct solution));
    sol->convexHull = newList();
    sol->operationCount = 0;

    // Ensure there are at least 3 points
    if (p->numPoints < 3) {
        return sol;
    }

    // Find the point with the lowest y-coordinate
    long double lowest_pointX = 0;
    long double lowest_pointY = LDBL_MAX;
    int lowest_point;
    for (int i = 0; i < p->numPoints; i++) {
        if (p->pointsY[i] < lowest_pointY) {
            lowest_pointX = p->pointsX[i];
            lowest_pointY = p->pointsY[i];
            lowest_point = i;
        } else if (p->pointsY[i] == lowest_pointY && p->pointsX[i] < lowest_pointX) {
            lowest_pointX = p->pointsX[i];
            lowest_pointY = p->pointsY[i];
            lowest_point = i;
        }
    }

    // Create priority queue
    struct pq *pq = createPQ();

    // Calculate polar angles and insert points into the priority queue
    for (int i = 0; i < p->numPoints; i++) {
        // Find polar angle
        long double dx = p->pointsX[i] - lowest_pointX;
        long double dy = p->pointsY[i] - lowest_pointY;
        if (dx != 0 && dy != 0) {
            long double angle = atan2l(dy, dx);
            insert(pq, i, angle);
        }
    }

    // Sort into list
    int *points = (int*)malloc(p->numPoints * sizeof(int));
    points[0] = lowest_point;
    for (int i = p->numPoints -  1; i > 0; i--) {
        points[i] = pull(pq);
    }

    // Push the first three points to the stack
    // Initialize an empty stack to store convex hull points
    struct stack *stack = createStack(points[0]);
    push(&stack, points[1]);
    push(&stack, points[2]);
    
    // Iterate over the remaining points
    for (int i = 3; i < p->numPoints; i++) {
        // While the current point and the two points below the top of the stack
        // make a non-left turn, pop the top of the stack       
        while (orientation(p, stack->below->item, stack->item, points[i]) != 2) {
            pop(&stack);
        }
        // Push the current point to the stack
        push(&stack, points[i]);
    }

    // Convert stack to linked list
    while (1) {
        // Pop an item from the stack
        int popped_point = pop(&stack);

        // Break if nothing left
        if (popped_point == -1) {
            break;
        }

        // Insert the popped point into the linked list
        insertHead(sol->convexHull, p->pointsX[popped_point], p->pointsY[popped_point]);
    }

    // Free memory
    free(points);
    free(stack);
    freePQ(pq);

    // Return solution
    return sol;

}


void freeSolution(struct solution *s){
    if(! s){
        return;
    }
    if(s->convexHull){
        freeList(s->convexHull);
    }
    free(s);
}


