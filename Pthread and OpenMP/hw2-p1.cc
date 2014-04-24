// --*-c++-*--

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"

int main(int argc, char *argv[])
{
  int N = (argc > 1 ? atoi(argv[1]) : DEFAULT_MATRIX_SIZE); 
  printf("matrix size: %d\n", N);

  int i = 0, j = 0;

  double **A = Make2DDoubleArray(N, N);
  double **B = Make2DDoubleArray(N, N);
  double **C = Make2DDoubleArray(N, N);
  double **D = Make2DDoubleArray(N, N);
 
  fillMatrix(N, A);
  fillMatrix(N, B);

  double elapse = 0;
  int numRuns = 10;

  for (i = 0; i < numRuns; i++) {
    double seconds = read_timer();
    serialMM(N, A, B, C);
    elapse += (read_timer() - seconds);
  }
  elapse /= numRuns; 
  printf("serial matrix multiplication: %lf\n", elapse);
 
  elapse = 0;
  for (i = 0; i < numRuns; i++) {
    double seconds = read_timer();
    pthreadMM(N, A, B, D);
    elapse += (read_timer() - seconds);
  }
  elapse /= numRuns; 
  printf("pthread matrix multiplication: %lf\n", elapse);

  if (matrix_compare(N, C, D) != 0) {
     printf("*** error ***\n");
     exit(-1);
  }

  return 0;
}
