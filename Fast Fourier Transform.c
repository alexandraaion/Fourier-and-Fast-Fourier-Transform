
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <pthread.h>
#include <math.h>

int Nthread;
int N;

double* xin;
double complex* xinc;
double complex* out;

void _fft(double complex buf[], double complex out[], int n, int step)
{

	if (step < n) {
		_fft(out, buf, n, step * 2);
		_fft(out + step, buf + step, n, step * 2);
 
		for (int i = 0; i < n; i += 2 * step) {
			double complex t = cexp(-I * M_PI * i / n) * out[i + step];
			buf[i / 2]     = out[i] + t;
			buf[(i + n)/2] = out[i] - t;
		}
	}
}



void* threadFunction4(void* args){
	int thread_id = *(int*)args;
	_fft(xinc + thread_id, out + thread_id, N, 4);

	return NULL;
}

void* threadFunction2(void* args){
	
	int thread_id = *(int*)args;
	_fft(out + thread_id, xinc + thread_id, N, 2);

	return NULL;
} 

 
void fft(double complex buf[], int n, int step){
	out = (double complex*) malloc(n * sizeof(double complex));;
 	int i;

 	for (i = 0; i < n; i++) 
 		out[i] = buf[i];

	pthread_t tid[Nthread];
	int thread_id[Nthread];

	for(i = 0;i < Nthread; i++)
		thread_id[i] = i;

	//apelam functia threadFunction in functie de cate threaduri avem

	if(Nthread == 1){
		_fft(xinc, out, N, step);
	}

	if(Nthread == 2){
		for(i = 0; i < Nthread; i++){
			pthread_create(&(tid[i]), NULL, threadFunction2, &(thread_id[i]));
		}
		
		for(i = 0; i < Nthread; i++){
					pthread_join(tid[i], NULL);
		}

	}

	if(Nthread == 4){
		step = 2;
		for(i = 0; i < Nthread; i++){
			pthread_create(&(tid[i]), NULL, threadFunction4, &(thread_id[i]));
		}

		for(i = 0; i < Nthread; i++){
			pthread_join(tid[i], NULL);
		}

		for (int i = 0; i < N; i += 2 * step){
			double complex t = cexp(-I * M_PI * i / N) * xinc[i + step];
			out[i / 2]     = xinc[i] + t;
			out[(i + N)/2] = xinc[i] - t;
		}

		for (int i = 0; i < N; i += 2 * step){
			double complex t = cexp(-I * M_PI * i / N) * xinc[i + step + 1];
			out[i / 2 + 1]     = xinc[i + 1] + t;
			out[(i + N)/2 + 1] = xinc[i + 1] - t;
		}	
	}
			
}	

int main(int argc, char * argv[]) {
	
	//deschidem fisierul de input si citim valorile
	FILE *fptrin;
	fptrin = fopen(argv[1], "r");

	if(fptrin == NULL){
	 	return -1;
	}

	Nthread = atoi(argv[3]);

	int i;

	if (fscanf(fptrin, "%d", &N) == 0){
	 	return -1;
	}

	xin = (double *)malloc(sizeof(double) * N);
	xinc = (double complex*)malloc(sizeof(double complex) * N);

	for(i = 0; i < N; i++){
	 	if (fscanf(fptrin, "%lf", &xin[i])== 0){
	 		return -1;
	 	}
	 	xinc[i] = (complex) xin[i];
	 }

	fclose(fptrin);
	int stepTh = 1;

	fft(xinc, N, stepTh);

	for (int i = 0; i < N; i += 2 * stepTh){
			double complex t = cexp(-I * M_PI * i / N) * out[i + stepTh];
			xinc[i / 2] = out[i] + t;
			xinc[(i + N)/2] = out[i] - t;
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

		if (fprintf(fptrout, "%.6lf %.6lf\n", creal(xinc[i]), cimag(xinc[i])) == 0){
			return -1;
	 	}
	}

	fclose(fptrin);
	free(xinc);
	free(xin);
	free(out);

	return 0;
}
