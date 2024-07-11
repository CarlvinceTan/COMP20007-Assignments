/*
graph.c

Set of vertices and edges implementation.

Implementations for helper functions for graph construction and manipulation.

Skeleton written by Grady Fitzpatrick for COMP20007 Assignment 1 2024

Modified by Carlvince Tan
*/
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "graph.h"
#include "utils.h"
#include "pq.h"

#define INITIALEDGES 32

struct edge;

/* Definition of a graph. */
struct graph {
  int numVertices;
  int numEdges;
  int allocedEdges;
  struct edge **edgeList;
};

/* Definition of an edge. */
struct edge {
  int start;
  int end;
  int cost;
};

struct graph *newGraph(int numVertices){
  struct graph *g = (struct graph *) malloc(sizeof(struct graph));
  assert(g);
  /* Initialise edges. */
  g->numVertices = numVertices;
  g->numEdges = 0;
  g->allocedEdges = 0;
  g->edgeList = NULL;
  return g;
}

/* Adds an edge to the given graph. */
void addEdge(struct graph *g, int start, int end, int cost){
  assert(g);
  struct edge *newEdge = NULL;
  /* Check we have enough space for the new edge. */
  if((g->numEdges + 1) > g->allocedEdges){
    if(g->allocedEdges == 0){
      g->allocedEdges = INITIALEDGES;
    } else {
      (g->allocedEdges) *= 2;
    }
    g->edgeList = (struct edge **) realloc(g->edgeList,
      sizeof(struct edge *) * g->allocedEdges);
    assert(g->edgeList);
  }

  /* Create the edge */
  newEdge = (struct edge *) malloc(sizeof(struct edge));
  assert(newEdge);
  newEdge->start = start;
  newEdge->end = end;
  newEdge->cost = cost;

  /* Add the edge to the list of edges. */
  g->edgeList[g->numEdges] = newEdge;
  (g->numEdges)++;
}

/* Returns a new graph which is a deep copy of the given graph (which must be 
  freed with freeGraph when no longer used). */
struct graph *duplicateGraph(struct graph *g){
  struct graph *copyGraph = (struct graph *) malloc(sizeof(struct graph));
  assert(copyGraph);
  copyGraph->numVertices = g->numVertices;
  copyGraph->numEdges = g->numEdges;
  copyGraph->allocedEdges = g->allocedEdges;
  copyGraph->edgeList = (struct edge **) malloc(sizeof(struct edge *) * g->allocedEdges);
  assert(copyGraph->edgeList || copyGraph->numEdges == 0);
  int i;
  /* Copy edge list. */
  for(i = 0; i < g->numEdges; i++){
    struct edge *newEdge = (struct edge *) malloc(sizeof(struct edge));
    assert(newEdge);
    newEdge->start = (g->edgeList)[i]->start;
    newEdge->end = (g->edgeList)[i]->end;
    newEdge->cost = (g->edgeList)[i]->cost;
    (copyGraph->edgeList)[i] = newEdge;
  }
  return copyGraph;
}

/* Frees all memory used by graph. */
void freeGraph(struct graph *g){
  int i;
  for(i = 0; i < g->numEdges; i++){
    free((g->edgeList)[i]);
  }
  if(g->edgeList){
    free(g->edgeList);
  }
  free(g);
}

struct solution *graphSolve(struct graph *g, enum problemPart part,
  int numLocations, int startingLocation, int finalLocation){
  struct solution *solution = (struct solution *)
    malloc(sizeof(struct solution));
  assert(solution);
  if(part == PART_A){
    /* 2A IMPLEMENTATION */

    // Initialize variables
    int *damage = (int *)malloc(numLocations * sizeof(int));
    int *visited = (int *)calloc(numLocations, sizeof(int));

    for (int i = 0; i < numLocations; i++) {
        damage[i] = INT_MAX;
        visited[i] = 0; 
    }

    // Set startingLocation distance to 0
    damage[startingLocation] = 0;
    
    // Commence Dijkstra's algorithm
    for (int count = 0; count < numLocations; count++) {
      // Find vertex with min distance
      int minDistance = INT_MAX;
      int minIndex = -1;
      for (int i = 0; i < numLocations; i++) {
        if (!visited[i] && damage[i] < minDistance) {
          minDistance = damage[i];
          minIndex = i;
        }
      }

      // Mark as visited
      visited[minIndex] = 1;

      // Update damage for adjacent vertices
      for (int i = 0; i < g->numEdges; i++) {
        struct edge *currentEdge = g->edgeList[i];
        // Current vertex is mentioned at start 
        // and Adjacent vertix hasn't been analysed
        if (currentEdge->start == minIndex 
            && !visited[currentEdge->end]) {
          // Compares against previous costs for vertix
          int newDamage = damage[minIndex] + 1;
          if (newDamage < damage[currentEdge->end]) {
            damage[currentEdge->end] = newDamage;
          }
        }
        // Current vertex is mentioned at end
        // Adjacent vertix hasn't been analysed
        else if (currentEdge->end == minIndex
            && !visited[currentEdge->start]) {
          // Compares against previous costs for vertix
          int newDamage = damage[minIndex] + 1;
          if (newDamage < damage[currentEdge->start]) {
            damage[currentEdge->start] = newDamage;
          }
        }
      }
    }

    // Transfer to solution
    solution->damageTaken = damage[finalLocation];

    // Free memory
    free(damage);
    free(visited);

  } else if(part == PART_B) {
    /* 2B IMPLEMENTATION */

    // Initialize variables
    int *costs = (int *)malloc(numLocations * sizeof(int));
    int *visited = (int *)calloc(numLocations, sizeof(int));

    for (int i = 0; i < numLocations; i++) {
        costs[i] = INT_MAX;
        visited[i] = 0; 
    }

    // Set startingLocation distance to 0
    costs[startingLocation] = 0;
    
    // Commence Dijkstra's algorithm
    for (int count = 0; count < numLocations; count++) {
      // Find vertex with min distance
      int minCost = INT_MAX;
      int minIndex = -1;
      for (int i = 0; i < numLocations; i++) {
        if (!visited[i] && costs[i] < minCost) {
          minCost = costs[i];
          minIndex = i;
        }
      }

      // Mark as visited
      visited[minIndex] = 1;

      // Update costs for adjacent vertices
      for (int i = 0; i < g->numEdges; i++) {
        struct edge *currentEdge = g->edgeList[i];
        // Current vertex is mentioned at start 
        // and Adjacent vertix hasn't been analysed
        if (currentEdge->start == minIndex 
            && !visited[currentEdge->end]) {
          // Compares against previous costs for vertix
          int newCost = costs[minIndex] + currentEdge->cost;
          if (newCost < costs[currentEdge->end]) {
            costs[currentEdge->end] = newCost;
          }
        }
        // Current vertex is mentioned at end
        // Adjacent vertix hasn't been analysed
        else if (currentEdge->end == minIndex
            && !visited[currentEdge->start]) {
          // Compares against previous costs for vertix
          int newCost = costs[minIndex] + currentEdge->cost;
          if (newCost < costs[currentEdge->start]) {
            costs[currentEdge->start] = newCost;
          }
        }
      }
    }

    // Transfer to solution
    solution->totalCost = costs[finalLocation];

    // Free memory
    free(costs);
    free(visited);

  } else if(part == PART_C) {
    /* 2C IMPLEMENTATION */

    // Create a priority queue to store candidate edges
    struct pq *pq = newPQ();

    // Initialize arrays
    int *visited = (int *)calloc(numLocations, sizeof(int));
    int *path = (int *)malloc(numLocations * sizeof(int));

    // Start from the specified starting location
    int currentVertex = startingLocation;
    visited[currentVertex] = 1;

    // Add starting location to path
    path[0] = currentVertex;

    // Initialize counter and cost
    int visitedCount = 1;
    solution->artisanCost = 0;

    // Iterate through all vertices
    while (visitedCount < numLocations) {
      // Add adjacent edges of the current vertex to the priority queue
      for (int i = 0; i < g->numEdges; i++) {
        struct edge *e = g->edgeList[i];
        if (e->start == currentVertex && !visited[e->end]) {
          enqueue(pq, e, e->cost);
        } else if (e->end == currentVertex && !visited[e->start]) {
          enqueue(pq, e, e->cost);
        }
      }

      // Find the minimum cost edge to an unvisited vertex
      struct edge *minEdge = NULL;
      while (!empty(pq)) {
        minEdge = (struct edge *)deletemin(pq);
        if (!visited[minEdge->start] || !visited[minEdge->end]){
          break;
        }
      }
      // Update current vertex and total cost
      if (minEdge) {
        currentVertex = visited[minEdge->start] ? minEdge->end : minEdge->start;
        solution->artisanCost += minEdge->cost;
        path[visitedCount++] = currentVertex;
        visited[currentVertex] = 1;
      } else {
        // No more unvisited vertices reachable from the current vertex
        break;
      }
    }

    // Free memmory
    free(visited);
    free(path);
    freePQ(pq);

  } else {
    /* 2D IMPLEMENTATION */

    // Initialize variables
    double *percentages = (double *)malloc(numLocations * sizeof(double));
    int *visited = (int *)calloc(numLocations, sizeof(int));

    for (int i = 0; i < numLocations; i++) {
        percentages[i] = DBL_MAX;
        visited[i] = 0; 
    }


    // Set startingLocation percentage to 1
    percentages[startingLocation] = 1;
    
    // Commence Dijkstra's algorithm
    for (int count = 0; count < numLocations; count++) {
      // Find vertex with min percentage
      double minDistance = INT_MAX;
      int minIndex = -1;
      for (int i = 0; i < numLocations; i++) {
        if (!visited[i] && percentages[i] < minDistance) {
          minDistance = percentages[i];
          minIndex = i;
        }
      }

      // Mark as visited
      visited[minIndex] = 1;

      // Update percentages for adjacent vertices
      for (int i = 0; i < g->numEdges; i++) {
        struct edge *currentEdge = g->edgeList[i];
        // Current vertex is mentioned at start 
        // and Adjacent vertix hasn't been analysed
        if (currentEdge->start == minIndex 
            && !visited[currentEdge->end]) {
          // Compares against previous percentages for vertix
          double newPercentage = percentages[minIndex] * (currentEdge->cost / 100.0 + 1);
          if (newPercentage < percentages[currentEdge->end]) {
            percentages[currentEdge->end] = newPercentage;
          }
        }
        // Current vertex is mentioned at end
        // Adjacent vertix hasn't been analysed
        else if (currentEdge->end == minIndex
            && !visited[currentEdge->start]) {
          // Compares against previous precentages for vertix
          double newPercentage = percentages[minIndex] * (currentEdge->cost / 100.0 + 1);
          if (newPercentage < percentages[currentEdge->start]) {
            percentages[currentEdge->start] = newPercentage;
          }
        }
      }
    }

    // Transfer to solution
    solution->totalPercentage = (int)((percentages[finalLocation] - 1) * 100);

    // Free memory
    free(percentages);
    free(visited);
  }
  return solution;
}


