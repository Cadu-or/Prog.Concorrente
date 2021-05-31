/**
 * Projeto final programacao concorrente - Problema da maquina de cafe
 * Carlos Eduardo de Oliveira Ribeiro
 * Matricula - 180099094
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define TRUE 1
#define N_F 15
#define MAQUINA 20

// Cores no terminal

#define VERMELHO "\033[1;31m"                                 
#define CIANO "\033[36m"
#define AMARELO "\033[33m"
#define RESET "\033[0;0m"

// Locks e variaveis de condicao utilizadas

pthread_mutex_t maquina_cafe = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fila_lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t funcionario_cond = PTHREAD_COND_INITIALIZER;

sem_t gerente_perm;         // Semaforo para avisar que o tecnico arrumou a maquina
sem_t gerente_sem;          // Semaforo para dar permissao ao gerente de usar a maquina
sem_t funcionario_sem;      // Semaforo para dar permissao para os funcionarios de usar a maquina
sem_t tecnico;              // Semaforo para acordar o tecnico
sem_t conserto;             // Semaforo com o numero de permissoes para a maquina de cafe quebrar

int fila = 0;               // Fila de funcionarios
int gerente = 0;            // Flag para ver se existe gerente
int tecnico_acordou = 0;    // Flag para saber se o tecnico esta consertando a maquina

void * f_tecnico(void *arg){
    while(TRUE){
        sem_wait(&tecnico); // Acorda o tecnico
        tecnico_acordou = 1;

        pthread_mutex_lock(&maquina_cafe);                       // Acesso a maquina de cafe
            printf(VERMELHO "Tecnico consertando\n" RESET);
            sleep(8);
            for(int i = 0; i < MAQUINA; i++){       // Conserta a maquina de cafe para quebrar com a mesma quantidade de cafe
                sem_post(&conserto);
            }
            pthread_cond_broadcast(&funcionario_cond);       // Acorda os funcionarios que a maquina voltou
            sem_post(&gerente_sem);                         // Avisa para o gerente que a maquina voltou
            tecnico_acordou = 0;
        pthread_mutex_unlock(&maquina_cafe);
        
        sem_post(&funcionario_sem);        // Libera o funcionario que estava esperando na fila do cafe
    }
}


void * f_gerente(void *arg){
    int id = *(int*)arg;
    while(TRUE){
        sleep(5);
        if(tecnico_acordou == 1){
            printf(AMARELO "Maquina quebrada, gerente vai embora\n" RESET);
            sem_wait(&gerente_sem);                 // Gerente sabe que a maquina ta quebrada

        }else{
            printf(AMARELO "Gerente quer cafe\n" RESET);    
            gerente = 1;                            // Mostra aos funcionarios que o gerente quer cafe (preferencia)
        
            sem_wait(&gerente_perm);                // Gerente espera a permissao de quem esta usando a maquina
                if(sem_trywait(&conserto) == 0){    // Verifica se a maquina esta quebrada
                    gerente = 0;                    // Tira a preferencia ja que o gerente foi servido
                    printf(AMARELO "Gerente se servindo\n" RESET);
                    sleep(1);
                }else{
                    printf(VERMELHO "Gerente foi chamar o tecnico\n" RESET);  
                    sem_post(&tecnico);         // Gerente foi acordar o tecnico 
                    gerente = 0;                // Gerente voltou para a sala
                }

            printf(AMARELO "Vou dar permissao para um funcionario\n" RESET);
            sem_post(&funcionario_sem); // Gerente dando permissao para o proximo da fila

        }

        sleep(20); // Sleep para simular o gerente voltando para a sala e tomar o cafe dele
    }
}

void * f_funcionario(void *arg){
    int id = *(int*)arg;
    while(TRUE){
        sleep(1);
        pthread_mutex_lock(&fila_lock);     // Lock para a fila de funcionarios
            fila++;                         // incremento da fila
            sleep(1);
            if(tecnico_acordou == 1){       // Variavel para saber se o tecnico esta consertando a maquina
                printf(CIANO "Maquina quebrada, funcionario %d espera\n" RESET, id);
                pthread_cond_wait(&funcionario_cond, &fila_lock); // Funcionario esperando a maquina de cafe ficar pronta
            }
        pthread_mutex_unlock(&fila_lock);
        
        sem_wait(&funcionario_sem);                  // Esperando a permissao para utilizar a maquina
        pthread_mutex_lock(&maquina_cafe);                    // Lock para ter acesso a maquina
            fila--;                                  // Decremento da fila
            if(sem_trywait(&conserto) == 0){         // Decremento do semaforo conserto para a maquina quebrar
                printf("Funcionario %d se servindo, fila: %d\n", id, fila);
                sleep(1);
                if(gerente == 1){                    // Caso tenha gerente esperando, da permissao para ele ser o proximo
                    printf("%d: Vou dar permissao para o gerente\n", id);
                    sem_post(&gerente_perm);
                }else{                              // Caso nao tenha gerente vai o proximo funcionario 
                    sem_post(&funcionario_sem);
                }
            }else{
                printf(VERMELHO "Funcionario %d: Chamando o tecnico\n" RESET, id); // Caso nao tenha mais permissao em conserto
                sem_post(&tecnico);     // Acorda o tecnico
            }

        pthread_mutex_unlock(&maquina_cafe);

        sleep(10); // Sleep para simular os usuarios voltando para a sala e tomar o cafe deles
    }
}

int main(int argc, char **argv){
    int i;

    sem_init(&tecnico, 0, 0);               // Inicia o tecnico sem permissoes, dormindo
    sem_init(&conserto, 0, MAQUINA);        // Maquina com a quantidade de permissoes para ela quebrar
    sem_init(&gerente_perm, 0, 0);          // A espera do gerente para o tecnico arrumando
    sem_init(&funcionario_sem, 0, 1);       // A permissao do funcionario para usar a maquina de cafe
    sem_init(&gerente_sem, 0, 1);           // Permissao do gerente para usar a maquina de cafe

    pthread_t funcionarios[N_F];            // Threads funcionarios
    pthread_t gerente;                      // Thread do gerente
    pthread_t tecnico;                      // Thread do tecnico

    int *id;

    for(i = 0; i < N_F; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
	    pthread_create(&(funcionarios[i]), NULL, f_funcionario, (void *)(id));          // Criacao das threads
    }

    pthread_create(&(gerente), NULL, f_gerente, (void*)(id));
    pthread_create(&(tecnico), NULL, f_tecnico, (void*)(id));
    
    pthread_join(funcionarios[0], NULL);                    // Inicializacao das threads

    return 0;
}