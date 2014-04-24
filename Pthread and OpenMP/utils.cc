// --*-c++-*--

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"

double** Make2DDoubleArray(int arraySizeX, int arraySizeY)
{
  double** theArray;
  theArray = (double**) malloc(arraySizeX*sizeof(double*));
  int i = 0;

  for (i = 0; i < arraySizeX; i++)
    theArray[i] = (double*) malloc(arraySizeY*sizeof(double));

  return theArray;
}

void fillMatrix(int size, double** matrix)
{
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      matrix[i][j]= (double)(min(i+1,j+1));
    }
  }
}

int factor_check(int N, double **matrix)
{
  int error = 0;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (matrix[i][j] != 1.0) {
	printf("%lf\n", matrix[i][j]);
        error = 1;
        return error;
      }
    }
  }
  return error;
}

int matrix_compare(int N, double **A, double **B)
{
  int error = 0;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (A[i][j] != B[i][j]) {
        error = 1;
        return error;
      }
    }
  }
  return error;
}

int print(int n, double **C) 
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n ; j++) {
      printf("%lf ", C[i][j]);
    }
    printf("\n");
  }

  return 0;
}
