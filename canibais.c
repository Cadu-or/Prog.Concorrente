/*
	Carlos Eduardo de Oliveira Ribeiro - 180099094
	Estudo dirigido 5 - Problema dos canibais
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

void *canibal(void*meuid);
void *cozinheiro(int m);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cozi = PTHREAD_COND_INITIALIZER;
pthread_cond_t cani = PTHREAD_COND_INITIALIZER;

int porcoes = 0;

void main(argc, argv)
int argc;
char *argv[];
{
	int erro;
	int i, n, m;
	int *id;

	pthread_t tid[MAXCANIBAIS];

	if(argc != 3){
		printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
		exit(1);
	}

	n = atoi (argv[1]); //número de canibais
	m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
	printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

	if(n > MAXCANIBAIS){
		printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
		exit(1);
	}

	for (i = 0; i < n; i++)  {
		id = (int *) malloc(sizeof(int));
		*id = i;
		erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

		if(erro){
			printf("erro na criacao do thread %d\n", i);
			exit(1);
		}
	}

	cozinheiro(m);
}

void * canibal (void* pi){
	while(1){
		pthread_mutex_lock(&lock);	// Lock para nao deixar nenhum canibal entrar mais de uma vez para pegar porcao.
			while(porcoes == 0){  	// Enquanto nao tem porcao pronta os canibais vao dormindo.
				printf("Canibal %d: Nao tem comida, a mimir...\n", *(int *)(pi));
				pthread_cond_wait(&cani, &lock);
			}
			
			printf("Porcoes: %d\n", porcoes); // Print da quantidade de porcoes disponivel.
			porcoes--;
			sleep(2);

			if(porcoes == 0){		// Caso as porcoes chegue a zero acorda o cozinheiro.
				printf("Canibal %d: Acorda o cozinheiro.\n", *(int *)(pi));
				pthread_cond_signal(&cozi);
			}
			
		pthread_mutex_unlock(&lock);


		printf("%d: vou comer a porcao que peguei\n", *(int *)(pi)); // Os canibais podem comer ao mesmo tempo.
		sleep(10);
	}
}

void *cozinheiro (int m){
	while(1){
		pthread_mutex_lock(&lock);
			//dormir enquanto tiver comida
			while(porcoes){ // Checa para ver se existe porcao, se existe o cozinheiro dorme
				printf("Cozinheiro: Tem comida, a mimir...\n");
				pthread_cond_wait(&cozi, &lock);
			}
		pthread_mutex_unlock(&lock);

			printf("Cozinheiro: Vou cozinhar\n"); // Cozinheiro esta cozinhando.
			sleep(20);

		pthread_mutex_lock(&lock); // Lock para atualizar o valor das porcoes
			porcoes = m;
			pthread_cond_broadcast(&cani);	// Acorda todos os canibais que estavam dormindo.

		pthread_mutex_unlock(&lock);
	}
}