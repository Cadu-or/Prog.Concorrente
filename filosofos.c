/* 
	Carlos Eduardo de Oliveira Ribeiro - 180099094
	Estudo Dirigido 8 - Problema do jantar dos filosofos
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id+N-1)%N
#define DIR(id)	(id+1)%N

sem_t lock;
sem_t s[N];

void *filosofos (void *arg);
int estado[N];

void pega_talher (int n);
void devolve_talher (int n);

void test(int n){
	if(estado[n] == 0 && estado[ESQ(n)] != 1 && estado[DIR(n)] != 1){ // ve se os vizinhos tao comendo e se o n esta com fome
		estado[n] = 1; // Esta comendo
		sem_post(&s[n]);
	}
}

int main () {
	int i;
	int *id;

	sem_init(&lock, 0, 1); // Inicializa o lock

	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 0);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
    for (i = 0; i < N ; i++) {
		id = (int *) malloc(sizeof(int));
	    *id = i;
	    pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
        printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher(int n){
	sem_wait(&lock); // exclusao mutua
		estado[n] = 0; // Esta com fome
		test(n);
	sem_post(&lock);

	sem_wait(&s[n]); // bloqueia se os garfos nao foram pegos
}

void devolve_talher(int n){
	sem_wait(&lock);
		estado[n] = 2; // Esta pensando
		test(ESQ(n)); // ve se o vizinho da esquerda pode comer
		test(DIR(n)); // ve se o vizinho da direita pode comer
    sem_post(&lock);
}