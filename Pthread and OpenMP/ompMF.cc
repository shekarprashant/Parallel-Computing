// --*-c++-*--
#include <omp.h>

#define NUM_THREADS 4

int ompMF(int ld, double** matrix)
{ 
	// please change the code below to parallel implementation
	double diag;
	int error = 0;
	int i = 0, j = 0, k = 0;


	#pragma omp parallel firstprivate(matrix, ld) private(i, j, k) num_threads(NUM_THREADS)
	for (k=0; k<ld; k++)  {

		if (matrix[k][k] == 0.0 || error == 1) {
			error = 1;
			break;
		}

		diag = 1.0 / matrix[k][k];

		#pragma omp for schedule(static)
		for (int i=k+1; i < ld; i++) {
			matrix[k][i] = diag * matrix[k][i];
		}

		#pragma omp for schedule(static)
		for (int i=k+1; i<ld; i++) {
			for (int j=k+1; j<ld; j++) {
					matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
				}
			}
	}	

	return error;



}
