// --*-c++-*--

void optMM(int N, double **A, double **B, double **C)
{
	// make the code below optimized matrix multiplation routine

	// you should modify or tweak some/all these to get the better performance
	int nrolls = 8;
	int blocksize = 32;

	int i = 0, j = 0, k = 0, ii = 0, jj = 0, kk = 0;
	int inc = N/blocksize;
	inc = inc > 1 ? inc : 2;
	inc += inc%2;


	for (i = 0; i < N; i+=inc) {
		for (j = 0; j < N; j+=inc) {
			for (k = 0; k < N; k+=inc) {
				for (ii = i; ii + 1 < i + inc && ii + 1 < N; ii+=2) {
					for (jj = j; jj + 1 < j + inc && jj + 1 < N; jj+=2) {
						for (kk = k; kk + 1 < k + inc && kk + 1 < N; kk+=2) {

							C[ii][jj] += A[ii][kk] * B[kk][jj] + A[ii][kk + 1] * B[kk + 1][jj];
							C[ii][jj + 1] += A[ii][kk] * B[kk][jj + 1] + A[ii][kk + 1] * B[kk + 1][jj + 1];
							C[ii + 1][jj] += A[ii + 1][kk] * B[kk][jj] + A[ii + 1][kk + 1] * B[kk + 1][jj];
							C[ii + 1][jj + 1] += A[ii + 1][kk] * B[kk][jj + 1] + A[ii + 1][kk + 1] * B[kk + 1][jj + 1];

						}
					}
				}
				
			}
		}
	}

	if (ii == N-1){
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				if (i < N-1 && j < N-1){
					C[N-1][i] += A[N-1][j]*B[j][i];
					C[i][N-1] += A[i][j]*B[j][N-1];
				}
				C[i][j] += A[i][N-1]*B[N-1][j];
			}
			if (i < N-1)
				C[N-1][N-1] += A[N-1][i]*B[i][N-1];
		}

		
	}
	return;
}

