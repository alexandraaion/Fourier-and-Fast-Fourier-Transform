#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <pthread.h>
#include <math.h>

int Nthread;
int N;

double* xin;
double complex* xout;

void* threadFunction(void *args)
{

	int thread_id = *(int*)args;

	int start = thread_id * ceil((double)N / Nthread);
	int end, k, n;

	end = fmin(N,(thread_id + 1)*ceil((double)N/Nthread));

	for(k = start; k < end; k++){
	 	xout[k] = 0;
	 	for(n = 0; n < N; n++){
	 		xout[k] += xin[n]* cexp(((-2*M_PI*I)/N)*k*n);
	 	}
	 }

	return NULL;
}


int main(int argc, char * argv[]) {
	FILE *fptrin;
	fptrin = fopen(argv[1], "r");
	Nthread = atoi(argv[3]);

	if(fptrin == NULL){
		return -1;
	}

	int i;

	if (fscanf(fptrin, "%d", &N) == 0){
	 	return -1;
	}
	xin = (double *)malloc(sizeof(double) * N);
	xout = (complex double *)malloc(sizeof(complex double) * N);

	for(i = 0; i < N; i++){

		if (fscanf(fptrin, "%lf", &xin[i]) == 0){
			return -1;
	 	}
	 }

	 fclose(fptrin);

	pthread_t tid[Nthread];
	int thread_id[Nthread];

	for(i = 0;i < Nthread; i++)
		thread_id[i] = i;

	for(i = 0; i < Nthread; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(thread_id[i]));
	}

	for(i = 0; i < Nthread; i++) {
		pthread_join(tid[i], NULL);
	}



	 FILE *fptrout;
	 fptrout = fopen(argv[2], "w");

	 if(fptrout == NULL){
	 	return -1;
	 }

	 if (fprintf(fptrout, "%d\n", N) == 0){
	 	return -1;
	 }

	 for(i = 0; i < N; i++){
	 
	 	if (fprintf(fptrout, "%.6lf %.6lf\n", creal(xout[i]), cimag(xout[i])) == 0){
	 		return -1;
	 	}
	 }

	 fclose(fptrin);
	 free(xin);
	 free(xout);


	return 0;
}
