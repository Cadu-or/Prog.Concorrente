/*
	Carlos Eduardo de Oliveira Ribeiro - 180099094
	Estudo dirigido 11 - Problema da soma dos elementos de uma matriz
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000 /* maximum matrix size */

pthread_barrier_t barrier;
int size;
int ans[MAXSIZE]; // Matriz que que contem a soma das linhas

int matrix1[MAXSIZE][MAXSIZE];

void *Worker(void *);

int main(int argc, char *argv[]){
	int i, j;

	size = atoi(argv[1]);

	if (size > MAXSIZE){						// Verificacao se o valor eh muito grande
		printf("Tamanho muito grande!\n");
		return 0;
	}

	pthread_t workerid[size];

	pthread_barrier_init(&barrier, NULL, size); // Inicializacao da barreira

	for (i = 0; i < size; i++)
		for (j = 0; j < size; j++)
			matrix1[i][j] = 1; 					// Preenche a matriz com o valor 1

	int *id;
	for (i = 0; i < size; i++){
		id = (int *)malloc(sizeof(int));
		*id = i;
		pthread_create(&workerid[i], NULL, Worker, (void *)(id));
	}

	for (i = 0; i < size; i++){
		if (pthread_join(workerid[i], NULL)){
			printf("\n ERROR joining thread");
			exit(1);
		}
	}
	printf("Bye!\n");
}

void *Worker(void *arg){
	int myid = *(int *)(arg);
	int j, k;
	int self = pthread_self();
	long long int sum = 0;
	printf("worker %d (pthread id %d) has started\n", myid, self);
	
	for (k = 0; k < size; k++){
		ans[myid] += matrix1[myid][k];   // Soma as linhas da matriz
	}

	pthread_barrier_wait(&barrier); // Sinaliza que o processo de soma terminou

	if (myid == 0){
		printf("\n");
		for (j = 0; j < size; j++){
			for (k = 0; k < size; k++){
				printf("%d ", matrix1[j][k]);
			}
			printf("\n");
		}
		for(int k = 0; k < size; k++){
			sum += ans[k];					// Soma os valores de cada linha e obtem o resultado final
		}
		printf("%lld\n", sum);
	}
}
