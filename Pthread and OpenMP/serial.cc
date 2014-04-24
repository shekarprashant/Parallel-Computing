// --*-c++-*--

void serialMM(int N, double **A, double **B, double **C)
{
   int i = 0, j = 0, k = 0;

   for (i = 0; i < N; i++) {
     for (j = 0; j < N; j++) {
       for (k = 0; k < N; k++) {
         C[i][j] = C[i][j] + A[i][k] * B[k][j];
       }
     }
   }

   return;
}

int serialMF(int ld, double **matrix)
{ 
  double diag;
  int error = 0;

  for (int k = 0; k < ld; k++)  {
    if (matrix[k][k] == 0.0) {
      error = 1;
      return error;
    }
    diag = 1.0 / matrix[k][k];
    for (int i = k+1; i < ld; i++) {
      matrix[k][i] = diag * matrix[k][i];
    }
    for (int i = k+1; i < ld; i++) {
      for (int j = k+1; j < ld; j++) {
        matrix[i][j] = matrix[i][j] - matrix[i][k] * matrix[k][j];
      }
    }
  }	

  return error;
}
