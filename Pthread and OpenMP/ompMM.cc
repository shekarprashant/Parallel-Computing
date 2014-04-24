// --*-c++-*--
#include <omp.h>
#include "defs.h"

#define NUM_THREADS 4

void ompMM(int N, double **A, double **B, double **C)
{
	// please change the code below to parallel implementation
	int i = 0, j = 0, k = 0;

	#pragma omp parallel for firstprivate(A, B, C, N) private(i, j, k) num_threads(NUM_THREADS) schedule(static, 1)
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				C[i][j] = C[i][j] + A[i][k] * B[k][j];
			}
		}
	}


	return;
}
