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
 
  double elapse = 0;
  int numRuns = 10;

  for (i = 0; i < numRuns; i++) {
    fillMatrix(N, A);
    double seconds = read_timer();
    serialMF(N, A);
    elapse += (read_timer() - seconds);
    if (factor_check(N, A) != 0) {
       printf("*** error ***\n");
       exit(-1);
    }
  }
  elapse /= numRuns; 
  printf("serial matrix factorization: %lf\n", elapse);
 
  elapse = 0;
  for (i = 0; i < numRuns; i++) {
    fillMatrix(N, A);
    double seconds = read_timer();
    ompMF(N, A);
    elapse += (read_timer() - seconds);
    if (factor_check(N, A) != 0) {
       printf("*** error ***\n");
       exit(-1);
    }
  }
  elapse /= numRuns; 
  printf("openMP matrix factorization: %lf\n", elapse);

  return 0;
}
