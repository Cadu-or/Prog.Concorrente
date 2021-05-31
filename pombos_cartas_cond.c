/* 
    Carlos Eduardo de Oliveira Ribeiro - 180099094
    Estudo dirigido 10 - Problema do pombo correio usando variaveis de condicao.
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

int contador = 0;
int local = 0;

pthread_cond_t usuario = PTHREAD_COND_INITIALIZER;
pthread_cond_t pombo = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

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
        pthread_mutex_lock(&lock);              
            while(contador != CARTAS){              // Enquanto nao tiver cartas suficientes na mochila do pombo o pombo dorme
                printf("Pombo dormindo\n");
                pthread_cond_wait(&pombo, &lock);
            }
            printf("Pombo indo da cidade A até a B\n");     // Pombo se locomove para a cidade B
            local = 1;                                      // Altera o lugar do pombo
        pthread_mutex_unlock(&lock);

        sleep(5);                                   // Sleep para atrasar a chegada do pombo ao local A 

        pthread_mutex_lock(&lock);
            printf("Pombo voltou para a cidade A\n");
            local = 0;                              // Flag indicadora, mostrando que o pombo esta na cidade certa
            contador = 0;                           // Contador da quantidade de cartas na mochila

            printf("Acordando os usuários\n"); 
            pthread_cond_broadcast(&usuario);       // Acordando os usuarios pois o pombo ja esta disponivel a receber cartas.

        pthread_mutex_unlock(&lock);
    }
}

void * f_usuario(void *arg){
    int id = *((int *) arg );
    while(1){
        sleep(2);
        pthread_mutex_lock(&lock);
            printf("Usuario %d escrevendo\n", id);  // Usuario escrevendo uma carta
            sleep(1);

            while( local == 1 || contador == CARTAS){ //Caso o pombo não esteja em A ou a mochila estiver cheia, então o usuario dorme	
                printf("Pombo não esta no local ou a mochila ta cheia, usuario %d esta dormindo\n", id);
                pthread_cond_wait(&usuario, &lock);
            }

            contador++;
            printf("Usuario %d colocou a carta na mochila do pombo", id); //Posta sua carta na mochila do pombo
            printf("\t\tMochila %d\n", contador);
            sleep(1);

            if(contador == CARTAS){ //Caso a mochila fique cheia, acorda o pombo
                pthread_cond_signal(&pombo);
            }

        pthread_mutex_unlock(&lock);
    }
}
