/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 2-related data structures and 
        functions.
    
    Sample solution implemented by Grady Fitzpatrick
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"
#include "prefixTree.h"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    /* Part B onwards so set as empty. */
    p->partialString = NULL;

    int wordCount = 0;
    int wordAllocated = 0;
    char *dictText = NULL;
    char **words = NULL;

    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&dictText, &allocated, '\0', dictFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    char *boardText = NULL;
    /* Reset allocated marker. */
    allocated = 0;
    success = getdelim(&boardText, &allocated, '\0', boardFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading board file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int dictTextLength = strlen(dictText);

    /* Count words present. */
    while(progress < dictTextLength){
        char *word = NULL;
        int nextProgress;
        /* Read each value into the dictionary. */
        if(progress == 0){
            /* First line. */
            int wordNumberGuess;
            assert(sscanf(dictText + progress, "%d %n", &wordNumberGuess, &nextProgress) == 1);
            /* Will fail if integer missing from the start of the words. */
            assert(nextProgress > 0);
            if(wordNumberGuess > 0){
                wordAllocated = wordNumberGuess;
                words = (char **) malloc(sizeof(char *) * wordAllocated);
                assert(words);
            }
            progress += nextProgress;
            continue;
        } else {
            char *newline_pos = strtok(dictText + progress, "\n");
            word = strdup(newline_pos);
            nextProgress = strlen(newline_pos) + 1;
            assert(nextProgress > 0);
            progress += nextProgress;
        }

        /* Check if more space is needed to store the word. */
        if(wordAllocated <= 0){
            words = (char **) malloc(sizeof(char *) * INITIALWORDSALLOCATION);
            assert(words);
            wordAllocated = INITIALWORDSALLOCATION;
        } else if(wordCount >= wordAllocated){
            words = (char **) realloc(words, sizeof(char *) * 
                wordAllocated * 2);
            assert(words);
            wordAllocated = wordAllocated * 2;
            /* Something has gone wrong if there's not sufficient word 
                space for another word. */
            assert(wordAllocated > wordCount);
        }

        words[wordCount] = word;
        wordCount++;
    }
    
    /* Done with dictText */
    if(dictText){
        free(dictText);
    }
    
    /* Now read in board */
    progress = 0;
    int dimension = 0;
    int boardTextLength = strlen(boardText);
    /* Count dimension with first line */
    while(progress < boardTextLength){
        /* Count how many non-space characters appear in line. */
        if(boardText[progress] == '\n' || boardText[progress] == '\0'){
            /* Reached end of line. */
            break;
        }
        if(isalpha(boardText[progress])){
            dimension++;
        }
        progress++;
    }

    assert(dimension > 0);

    /* Check each line has the correct dimension. */
    for(int i = 1; i < dimension; i++){
        int rowDim = 0;
        if(boardText[progress] == '\n'){
            progress++;
        }
        while(progress < boardTextLength){
            /* Count how many non-space characters appear in line. */
            if(boardText[progress] == '\n' || boardText[progress] == '\0'){
                /* Reached end of line. */
                break;
            }
            if(isalpha(boardText[progress])){
                rowDim++;
            }
            progress++;
        }
        if(rowDim != dimension){
            fprintf(stderr, "Row #%d had %d letters, different to Row #1's %d letters.\n", i + 1, rowDim, dimension);
            assert(rowDim == dimension);
        }
    }

    /* Define board. */
    char *boardFlat = (char *) malloc(sizeof(char) * dimension * dimension);
    assert(boardFlat);
    
    /* Reset progress. */
    progress = 0;
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            int nextProgress;
            assert(sscanf(boardText + progress, "%c %n", &boardFlat[i * dimension + j], &nextProgress) == 1);
            progress += nextProgress;
        }
    }

    char **board = (char **) malloc(sizeof(char **) * dimension);
    assert(board);
    for(int i = 0; i < dimension; i++){
        board[i] = &boardFlat[i * dimension];
    }

    // fprintf(stderr, "\n");

    /* The number of words in the text. */
    p->wordCount = wordCount;
    /* The list of words in the dictionary. */
    p->words = words;

    /* The dimension of the board (number of rows) */
    p->dimension = dimension;

    /* The board, represented both as a 1-D list and a 2-D list */
    p->boardFlat = boardFlat;
    p->board = board;

    /* For Part B only. */
    p->partialString = NULL;

    p->part = PART_A;

    return p;
}

struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
    FILE *partialStringFile){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(dictFile, boardFile);

    char *partialString = NULL;

    /* Part B has a string that is partially given - we assume it follows 
        word conventions for the %s specifier. */
    size_t len = 0;

    ssize_t read = getline(&partialString, &len, partialStringFile);
    assert(read != -1);  

    if (partialString[read - 1] == '\n') {
        partialString[read - 1] = '\0';
    }
    
    p->part = PART_B;
    p->partialString = partialString;

    return p;
}

struct problem *readProblemD(FILE *dictFile, FILE *boardFile){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(dictFile, boardFile);
    
    p->part = PART_D;
    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    switch(problem->part){
        case PART_A:
        case PART_D:
            assert(solution->foundWordCount == 0 || solution->words);
            for(int i = 0; i < solution->foundWordCount; i++){
                fprintf(outfileName, "%s\n", solution->words[i]);
            }
            break;
        case PART_B:
            assert(solution->foundLetterCount == 0 || solution->followLetters);
            for(int i = 0; i < solution->foundLetterCount; i++){
                if(isalpha(solution->followLetters[i])){
                    fprintf(outfileName, "%c\n", solution->followLetters[i]);
                } else {
                    fprintf(outfileName, " \n");
                }
            }
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        if(solution->followLetters){
            free(solution->followLetters);
        }
        if(solution->words){
            free(solution->words);
        }
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->words){
            for(int i = 0; i < problem->wordCount; i++){
                if(problem->words[i]){
                    free(problem->words[i]);
                }
            }
            free(problem->words);
        }
        if(problem->board){
            free(problem->board);
        }
        if(problem->boardFlat){
            free(problem->boardFlat);
        }
        if(problem->partialString){
            free(problem->partialString);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    s->foundWordCount = 0;
    s->words = NULL;
    s->foundLetterCount = 0;
    s->followLetters = NULL;
    
    return s;
}

/*
    Finds word within board using DFS
*/
void searchWord(struct prefixTree *root, struct problem *p, struct solution *s, int x, int y) {
    struct prefixTree *current = root;
    // Define all possible directions
    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    // Test all neighbouring cells
    for (int d = 0; d < 8; d++) {
        int newX = x + directions[d][0];
        int newY = y + directions[d][1];

        // Check adjacent cells 
        if (newX >= 0 && newX < p->dimension && newY >= 0 && newY < p->dimension &&     // Next cell not out of bounds
            current->children[(unsigned char)p->board[newX][newY]] != NULL &&           // Next cell exists within Prefix trie
            current->children[(unsigned char)p->board[newX][newY]]->mark == UNMARKED    // Not marked
            ) {
            struct prefixTree *nextNode = current->children[(unsigned char)p->board[newX][newY]];

            // Checks if string exists as word in prefix trie
            if (nextNode->children[(unsigned char)'\0'] != NULL) {
                // Check if the word is already found
                int alreadyFound = 0;
                for (int k = 0; k < s->foundWordCount; k++) {
                    if (strcmp(s->words[k], nextNode->children[(unsigned char)'\0']->string) == 0) {
                        alreadyFound = 1;
                        break;
                    }
                }

                // Add word to list
                if (!alreadyFound) {
                    s->foundWordCount++;
                    s->words = realloc(s->words, s->foundWordCount * sizeof(char *));
                    s->words[s->foundWordCount - 1] = nextNode->children[(unsigned char)'\0']->string;
                }
            }

            // Search deeper for next letter, mark matched letters thusfar as MARKED
            nextNode->mark = MARKED;
            searchWord(nextNode, p, s, newX, newY);
            nextNode->mark = UNMARKED;
        }
    }
}

/*
    Finds next possible letters for word using DFS
*/
void searchNextChars(struct prefixTree *current, struct problem *p, struct solution *s, int x, int y) {
    // Define all possible directions
    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    // If the current node's string matches the partial string length, find next possible letters
    if (strlen(current->string) == strlen(p->partialString)) {
        for (int d = 0; d < 8; d++) {
            int newX = x + directions[d][0];
            int newY = y + directions[d][1];

            // Check adjacent cells 
            if (newX >= 0 && newX < p->dimension && newY >= 0 && newY < p->dimension &&     // Next cell not out of bounds
                current->children[(unsigned char)p->board[newX][newY]] != NULL &&           // Next cell exists within Prefix trie
                current->children[(unsigned char)p->board[newX][newY]]->mark == UNMARKED    // Not a letter existing in string of branch being checked
                ) { 
                
                // Check if the letter is already found
                int alreadyFound = 0;
                for (int k = 0; k < s->foundLetterCount; k++) {
                    if (s->followLetters[k] == p->board[newX][newY]) {
                        alreadyFound = 1;
                        break;
                    }
                }

                // Add letter to list
                if (!alreadyFound) {
                    s->foundLetterCount++;
                    s->followLetters = realloc(s->followLetters, s->foundLetterCount * sizeof(char));
                    s->followLetters[s->foundLetterCount - 1] = p->board[newX][newY];
                }
            }
        }

        // Check if the current node marks the end of a word
        if (current->children[(unsigned char)'\0'] != NULL) {
            int alreadyFound = 0;
            for (int k = 0; k < s->foundLetterCount; k++) {
                if (s->followLetters[k] == '\0') {
                    alreadyFound = 1;
                    break;
                }
            }

            // Add '\0' to list if not already found
            if (!alreadyFound) {
                s->foundLetterCount++;
                s->followLetters = realloc(s->followLetters, s->foundLetterCount * sizeof(char));
                s->followLetters[s->foundLetterCount - 1] = '\0';
            }
        }
        return;
    }

    // Continue DFS if the current node's string does not yet match the partial string length
    char nextChar = p->partialString[strlen(current->string)];
    for (int d = 0; d < 8; d++) {
        int newX = x + directions[d][0];
        int newY = y + directions[d][1];
        if (newX >= 0 && newX < p->dimension && newY >= 0 && newY < p->dimension &&     // Next cell not out of bounds
            p->board[newX][newY] == nextChar &&                                         // Next cell is a valid next letter
            current->children[(unsigned char)p->board[newX][newY]] != NULL &&           // Next cell exists within Prefix trie
            current->children[(unsigned char)p->board[newX][newY]]->mark == UNMARKED    // Not a letter existing in string of branch being checked
            ) {
            
            struct prefixTree *nextNode = current->children[(unsigned char)p->board[newX][newY]];

            // Search deeper for next letter, mark matched letters thus far as MARKED
            nextNode->mark = MARKED;
            searchNextChars(nextNode, p, s, newX, newY);
            nextNode->mark = UNMARKED;
        }
    }
}

/*
    Mark Cells to prevent over/duplicate disabling for adjacent cells
*/
void markCell(Cell **matrix, int dimension, int x, int y, int dirX, int dirY) {
    if (x + dirX >= 0 && x + dirX < dimension && y + dirY >= 0 && y + dirY < dimension &&
        matrix[x + dirX][y + dirY].marked == UNMARKED) {
        matrix[x + dirX][y + dirY].marked = MARKED;
    }
}

/*
    Disable intemediary child nodes (node -> child -> grandchild) which share the same character
*/
void disableAdjacentCells(Cell **matrix, int dimension, int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    //  1 cell gap
    if (abs(dx) == 2 || abs(dy) == 2) {
        // Vertical case 
        if (dx == 0) {
            markCell(matrix, dimension, x1, y1, 0, dy / 2);
            markCell(matrix, dimension, x1, y1, 1, dy / 2);
            markCell(matrix, dimension, x1, y1, 2, dy / 2);
        } 
        // Horizontal case
        else if (dy == 0) {
            markCell(matrix, dimension, x1, y1, dx / 2, 0);
            markCell(matrix, dimension, x1, y1, dx / 2, 1);
            markCell(matrix, dimension, x1, y1, dx / 2, 2);
        }
        // L shape case
        else if ((abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2)){
            // Vertical
            if (abs(dx) < abs(dy)) {
                markCell(matrix, dimension, x1, y1, dx, dy / 2);
                markCell(matrix, dimension, x1, y1, 1, dy / 2);
            }
            // Horizontal 
            else {
                markCell(matrix, dimension, x1, y1, dx / 2, dy);
                markCell(matrix, dimension, x1, y1, dx / 2, 1);
            }
        }
    } 
    // Right next to each other
    else {
        // Diagonal
        if (abs(dx) == 1 && abs(dy) == 1) {
            markCell(matrix, dimension, x1, y1, dx, 0);
            markCell(matrix, dimension, x1, y1, 0, dy);
        } else {
            // Vertical
            if (dx == 0) {
                markCell(matrix, dimension, x1, y1, 0, 1);
                markCell(matrix, dimension, x1, y1, 2, 1);
                markCell(matrix, dimension, x1, y1, 0, dy);
                markCell(matrix, dimension, x1, y1, 2, dy);
            }
            // Horizonal
            else {
                markCell(matrix, dimension, x1, y1, 1, 0);
                markCell(matrix, dimension, x1, y1, dx, 0);
                markCell(matrix, dimension, x1, y1, 1, 2);
                markCell(matrix, dimension, x1, y1, dx, 2);
            }
        }
    }
}


/*
    Finds word within board using DFS whilst excluding words containing duplicateletters
*/
void searchWordOptimised(struct prefixTree *current, struct solution *s, Cell **matrix, int dimension, int x, int y) {
    int directions[3][3] = DIRECTIONS;

    // No more to be found in branch
    if (current->countChildren == 0) {  
        return;
    }

    // Test all neighbouring cells
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (matrix[x][y].directions[i][j] == 1 && !(i == 1 && j == 1)) {
                int newX = x + i - 1;
                int newY = y + j - 1;

                // Check adjacent cells
                if (newX >= 0 && newX < dimension && newY >= 0 && newY < dimension &&           // Next cell not out of bounds
                    current->children[(unsigned char) matrix[newX][newY].character] != NULL &&  // Next cell exists within Prefix trie
                    strchr(current->string, matrix[newX][newY].character) == NULL               // Not a letter existing in string of branch being checked                    
                    ) {
                    
                    // Point to next node
                    struct prefixTree *next = current->children[(unsigned char) matrix[newX][newY].character];
                    
                    // Update directions for this child
                    for (int n = 0; n < 3; n++) {
                        for (int m = 0; m < 3; m++) {
                            if (matrix[x][y].directions[n][m] == 1) {
                                int nX = x + n - 1;
                                int nY = y + m - 1;
                                if (!(n == 1 && m == 1) &&
                                    nX >= 0 && nX < dimension && nY >= 0 && nY < dimension &&       // Check not out of bounds
                                    matrix[newX][newY].character == matrix[nX][nY].character) {     // Same character
                                    disableAdjacentCells(matrix, dimension, newX, newY, nX, nY);    
                                }
                            }
                        }
                    }

                    // Unmark cells which were use to prevent duplicate-disabling
                    for (int n = -2; n <= 2; n++) {
                        for (int m = -2; m <= 2; m++) {
                            if (newX + n>= 0 && newX + n< dimension && newY + m>= 0 && newY + m< dimension) {
                                matrix[newX + n][newY + m].marked = UNMARKED;
                            }
                        }
                    }

                    // Checks if string exists as word in prefix trie
                    if (next->children[(unsigned char)'\0'] != NULL) {
                        // Check if the word is already found
                        int alreadyFound = 0;
                        for (int k = 0; k < s->foundWordCount; k++) {
                            if (strcmp(s->words[k], next->children[(unsigned char)'\0']->string) == 0) {
                                alreadyFound = 1;
                                break;
                            }
                        }

                        // Add word to list
                        if (!alreadyFound) {
                            s->foundWordCount++;
                            s->words = realloc(s->words, s->foundWordCount * sizeof(char *));
                            s->words[s->foundWordCount - 1] = next->children[(unsigned char)'\0']->string;
                            current->countChildren = 0;                                                         
                        }
                    }

                    // Search deeper for next letter, mark matched letters thusfar as MARKED
                    int beforeCount = s->foundWordCount;
                    next->mark = MARKED;
                    searchWordOptimised(next, s, matrix, dimension, newX, newY);
                    next->mark = UNMARKED;
                    int addedCount = s->foundWordCount - beforeCount;

                    // Update countChildren                                       
                    current->countChildren-= addedCount;
                }
            }
            // RESET directions
            matrix[x][y].directions[i][j] = directions[i][j];
        }
    }
}

/*
    Function to compare two strings lexicographically first then by length
*/
int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/*
    Function to compare two chars by value
*/
int compareChars(const void *a, const void *b) {
    return (*(char *)a - *(char *)b);
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p) {
    struct solution *s = newSolution(p);

    // Store Words into prefixTree
    struct prefixTree *root = newPrefixTree();
    for (int i = 0; i < p->wordCount; i++) {
        addWordToTree(root, p->words[i]);
    }

    // Change letters to lowercase
    for (int i = 0; i < p->dimension; i++) {
        for (int j = 0; j < p->dimension; j++) {
            p->board[i][j] = tolower(p->board[i][j]);
        }
    }

    // Finds starting points and applies DFS
    for (int i = 0; i < p->wordCount; i++) {
        for (int x = 0; x < p->dimension; x++) {
            for (int y = 0; y < p->dimension; y++) {
                if (p->board[x][y] == p->words[i][0]) {
                    searchWord(root->children[(unsigned char)p->board[x][y]], p, s, x, y);
                }
            }
        }
    }

    // Sort strings lexicographically then by length
    qsort(s->words, s->foundWordCount, sizeof(char*), compareStrings);

    return s;
}

struct solution *solveProblemB(struct problem *p){
    struct solution *s = newSolution(p);
    
    // Store Words into prefixTree
    struct prefixTree *root = newPrefixTree();
    for (int i = 0; i < p->wordCount; i++) {
        addWordToTree(root, p->words[i]);
    }

    // Change letters to lowercase
    for (int i = 0; i < p->dimension; i++) {
        for (int j = 0; j < p->dimension; j++) {
            p->board[i][j] = tolower(p->board[i][j]);
        }
    }

    // Finds starting points and applies DFS
    for (int x = 0; x < p->dimension; x++) {
        for (int y = 0; y < p->dimension; y++) {
            if (p->board[x][y] == p->partialString[0]) {
                searchNextChars(root->children[(unsigned char)p->board[x][y]], p, s, x, y);
            }
        }
    }

    // Sort chars by value
    qsort(s->followLetters, s->foundLetterCount, sizeof(char), compareChars);

    return s;
}

struct solution *solveProblemD(struct problem *p){
    struct solution *s = newSolution(p);

    // Store Words into prefixTree
    struct prefixTree *root = newPrefixTree();
    for (int i = 0; i < p->wordCount; i++) {
        addWordToTree(root, p->words[i]);
    }

    // Change letters to lowercase
    for (int i = 0; i < p->dimension; i++) {
        for (int j = 0; j < p->dimension; j++) {
            p->board[i][j] = tolower(p->board[i][j]);
        }
    }

    // Create Cell 2D matrix
    Cell **matrix = (Cell **) malloc(p->dimension * sizeof(Cell*));
    for (int i = 0; i < p->dimension; i++) {
        matrix[i] = (Cell *) malloc(p->dimension * sizeof(Cell));
    }

    // Transfer p->board to matrix and initialize directions
    int directions[3][3] = DIRECTIONS;
    for (int i = 0; i < p->dimension; i++) {
        for (int j = 0; j < p->dimension; j++) {
            matrix[i][j].character = p->board[i][j];
            for (int d = 0; d < 3; d++) {
                matrix[i][j].directions[d][0] = directions[d][0];
                matrix[i][j].directions[d][1] = directions[d][1];
                matrix[i][j].directions[d][2] = directions[d][2];
            }
            matrix[i][j].marked = UNMARKED;
        }
    }
    
    // Finds starting points and applies DFS
    for (int x = 0; x < p->dimension; x++) {
        for (int y = 0; y < p->dimension; y++) {
            for (int i = 0; i < p->wordCount; i++) {
                if (p->board[x][y] == p->words[i][0]) {
                    searchWordOptimised(root->children[(unsigned char)p->board[x][y]], s, matrix, p->dimension, x, y);
                }
            }
        }
    }
    
    // Sort strings lexicographically then by length
    qsort(s->words, s->foundWordCount, sizeof(char*), compareStrings);

    return s;
}




