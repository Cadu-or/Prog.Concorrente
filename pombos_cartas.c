/* 
    Carlos Eduardo de Oliveira Ribeiro - 180099094
    Estudo dirigido 9 - Problema do pombo correio usando semaforos.
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários
#define CARTAS 20 //quantidade de cartas na mochila
int contador = 0;
sem_t pombo, lock, espaco; 

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

    sem_init(&espaco, 0, CARTAS);
    sem_init(&lock, 0, 1);
    sem_init(&pombo, 0, 0);

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
    pthread_t pombo;
    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

    pthread_join(pombo,NULL);
}


void * f_pombo(void *arg){
    while(1){
        sem_wait(&pombo); // Pombo inicialmente em A entao ele espera as cartas

        sem_wait(&lock);
            printf("Pombo levou a carta até o B\n");
            sleep(5);

            for(int i = 0; i < CARTAS; i++){     // Pombo entrega as cartas
                sem_post(&espaco);              // Coloca os espacos para as cartas
            }

            printf("Pombo voltou para o ponto A\n");

            contador = 0; // contador de cartas
        sem_post(&lock);

    }
}

void * f_usuario(void *arg){
    int id = *((int *) arg );
    while(1){
        sleep(2);
        sem_wait(&espaco); // Verifica a existencia de espaco

        sem_wait(&lock); // Exclusao mutua
            contador++;
            printf("Usuarido %d escreveu uma carta \t\t\t Mochila: %d\n", id, contador); //Escreve uma carta
            sleep(1);

            if(contador == CARTAS){
                sem_post(&pombo); // Acorda o pombo para levar as cartas
            }

        sem_post(&lock);
    }
}
