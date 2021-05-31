/*
	Carlos Eduardo de Oliveira Ribeiro - 180099094
	Estudo dirigido 6 - Problema dos produtores e consumidores usando semaforos
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 3 // número de consumidores
#define N 5  //tamanho do buffer


void * produtor(void * meuid);
void * consumidor (void * meuid);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t vazio;
sem_t cheio;

int item = 0;


void main(int argc, char argv[]){

	int erro;
	int i, n, m;
	int *id;

	pthread_t tid[PR];

	sem_init(&vazio,N,N); // Inicializacao dos semaforos
	sem_init(&cheio,0,0); 
   
	for(i = 0; i < PR; i++){
		id = (int *) malloc(sizeof(int));
    	*id = i;
    	erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    	if(erro){
    		printf("erro na criacao do thread %d\n", i);
    		exit(1);
		}
	}

	pthread_t tCid[CN];

	for(i = 0; i < CN; i++){
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
		printf("Produzindo...\n");
		sleep(1);
		
		sem_wait(&vazio);  // Decresce o semaforo vazio.
		pthread_mutex_lock(&lock);	// Lock garantindo o a exclusao mutua ao acesso a regiao critica.
			item++;
			printf("Produziu, itens: %d\n", item); // Incrementa a quantidade de itens.
		pthread_mutex_unlock(&lock);
		sem_post(&cheio); // Incrementa o semaforo cheio.

	}

	pthread_exit(0);
  
}

void * consumidor (void* pi){
	while(1){
		sem_wait(&cheio); // Decresce o semaforo cheio. Caso esteja 0 fica em espera
		pthread_mutex_lock(&lock); // Lock garantindo o a exclusao mutua ao acesso a regiao critica.
			item--;
			printf("Consumindo...\n"); // Decrementa a quantidade de itens.

		pthread_mutex_unlock(&lock);
		sem_post(&vazio);  // Incrementa o semaforo vazio.

		printf("Consumiu, itens: %d\n", item);
		sleep(1);

	}

  	pthread_exit(0); 
}