#include <pthread.h>
#include <math.h>
#include "defs.h"

#define NUM_THREADS 4

struct params{

	int thread_num;
	int  N;

	int num_blocks;
	int block_size;
	int actual_N;

	int start_row;
	int end_row;

	int start_col;
	int end_col;

	double** A;
	double** B;
	double** C;
};


void* matrixmul(void* args)
{
	params* arg1 = (params *)args;
	int i = 0, j = 0, k = 0, l = arg1->thread_num, row = 0, col = 0;

	while (l < arg1->num_blocks)
	{
		if (l > arg1->thread_num)
		{
			
			col = (l*arg1->block_size)%(arg1->actual_N);
			row = ((l*arg1->block_size)/arg1->actual_N) * arg1->block_size;
			arg1->start_row = row;
			arg1->end_row = (row + arg1->block_size)<arg1->N ? (row + arg1->block_size) : arg1->N;

			arg1->start_col = col;
			arg1->end_col = (col + arg1->block_size)<arg1->N ? (col + arg1->block_size) : arg1->N;

		}

		for (i = arg1->start_row; i < arg1->end_row; i++) {
			for (j = arg1->start_col; j < arg1->end_col; j++) {
				for (k = 0; k < arg1->N; k++) {
					arg1->C[i][j] = arg1->C[i][j] + arg1->A[i][k] * arg1->B[k][j];
				}
			}
		}
		l += NUM_THREADS;

	}
	return NULL;
}

void
pthreadMM(int N, double **A, double **B, double **C)
{
	// please change the code below to parallel implementation

	pthread_t thread_ids[NUM_THREADS];
	params args[NUM_THREADS];

	int sqrt_threadnum = (int)ceil(sqrt(NUM_THREADS));
	sqrt_threadnum = sqrt_threadnum>N ? N : sqrt_threadnum;
	int block_size = N/sqrt_threadnum;

	int num_blocks = (int)(ceil((double)N/block_size)*ceil((double)N/block_size));
	int actual_N = (int)(ceil((double)N/block_size)) * block_size;

	int i = 0, row = 0, col = 0;


	for (i = 0; i < NUM_THREADS; i++)
	{
		args[i].thread_num = i;
		args[i].N = N;

		args[i].num_blocks = num_blocks;
		args[i].block_size = block_size;
		args[i].actual_N = actual_N;

		args[i].start_row = row;
		args[i].end_row = (row + block_size)<N ? (row + block_size) : N;

		args[i].start_col = col;
		args[i].end_col = (col + block_size)<N ? (col + block_size) : N;

		args[i].A = A;
		args[i].B = B;
		args[i].C = C;

		pthread_create(&thread_ids[i], NULL, matrixmul, (void *)(&args[i]));


		col += block_size;
		if (col >= N)
		{
			col = 0;
			row += block_size;
			if (row >= N)
			{
				for (int k = i+1; k < NUM_THREADS; k++)
					thread_ids[k] = -1;
				break;
			}
		}

	}

	for (i = 0; i < NUM_THREADS; i++)
	{
		if (thread_ids[i] != -1)
			pthread_join(thread_ids[i], NULL);
	}
	
	return;
}

