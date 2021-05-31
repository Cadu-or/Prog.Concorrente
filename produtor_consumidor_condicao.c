/*
	Carlos Eduardo de Oliveira Ribeiro - 180099094
	Estudo dirigido 4 - Problema dos produtores e consumidores usando variaveis de condicao
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t prod = PTHREAD_COND_INITIALIZER;
pthread_cond_t cons = PTHREAD_COND_INITIALIZER;

void * produtor(void * meuid);
void * consumidor (void * meuid);

int count = 0;

void main(int argc, char *argv[]){
	int erro;
	int i, n, m;
	int *id;

	pthread_t tid[PR];
   
	for (i = 0; i < PR; i++){
    	id = (int *) malloc(sizeof(int));
    	*id = i;
		erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    	if(erro){
      		printf("erro na criacao do thread %d\n", i);
      		exit(1);
    	}
  	}

  	pthread_t tCid[CN];

  	for (i = 0; i < CN; i++){
    	id = (int *) malloc(sizeof(int));
    	*id = i;
    	erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    	if(erro){
      		printf("erro na criacao do thread %d\n", i);
      		exit(1);
    	}
  	}
 
	pthread_join(tid[0],NULL);

}

void * produtor (void* pi){
	while(1){
		sleep(1);		// Sleep para conseguir que o produtor produza mais que o consumidor consiga cosumir
		pthread_mutex_lock(&lock);	
			while(count == N){		// Caso o numero do buffer chega ao maximo. Produtor dorme.
				printf("Produtor dormindo\n");
				pthread_cond_wait(&prod, &lock);
			}

			printf("Produtor produzindo... \n");	// Produtor produz
			sleep(1);
			
			count++;								// Incrementa o buffer.
			printf("Tamanho do buffer: %d\n", count);
			if(count == 1){							// Caso tenha algo no buffer acorda o consumidor para consumir do buffer.
				printf("Acorda o consumidor\n");		
				pthread_cond_signal(&cons);
			}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(0);
  
}

void * consumidor (void* pi){
	while(1){
		sleep(5);	// Sleep para conseguir que o produtor produza mais que o consumidor consiga cosumir
		pthread_mutex_lock(&lock);
			while(count == 0){			// Caso nao haja nada no buffer a ser consumido o consumidor dorme 
				printf("Consumidor dormindo\n");
				pthread_cond_wait(&cons, &lock);
			}

			printf("Consumidor consumindo... \n");	
			sleep(1);

			count--;						// Subtrai do buffer pois acabou de ser usado.
			printf("Buffer: %d\n", count);
			if(count == N-1){				// Caso o Buffer esteja cheio, acorda o produtor.
				printf("Acorda o produtor\n");
				pthread_cond_signal(&prod);
			}
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(0);
  
}
