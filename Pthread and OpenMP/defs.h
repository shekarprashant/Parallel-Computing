// --*-c++-*--

#ifndef _DEFS_H
#define _DEFS_H

#ifndef DEFAULT_MATRIX_SIZE 
#define DEFAULT_MATRIX_SIZE 100
#endif

// utility functions

double read_timer();
int factor_check(int N, double **matrix);
int matrix_compare(int N, double **A, double **B);
double** Make2DDoubleArray(int arraySizeX, int arraySizeY);
void fillMatrix(int size, double** matrix);
int print(int n, double **C);

// serial routines

void serialMM(int N, double **A, double **B, double **C);
void serialMF(int N, double **A);

// open MP routines

void ompMM(int N, double **A, double **B, double **C);
void ompMF(int N, double **A);

// pthread routines

void pthreadMM(int N, double **A, double **B, double **C);

// optimized MM

void optMM(int N, double **A, double **B, double **C);

#define min(x,y) ((x)<=(y)?(x):(y))
#define max(x,y) ((x)>=(y)?(x):(y))

#endif
