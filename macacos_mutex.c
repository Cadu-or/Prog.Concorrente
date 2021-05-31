/*
    Carlos Eduardo de Oliveira Ribeiro - 180099094
    Estudo dirigido 3 - Problema dos macacos 
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_travessia = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_veza = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_vezb = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_ga = PTHREAD_MUTEX_INITIALIZER;   

int mab = 0;
int mba = 0;
int ga = 0;


void * macacoAB(void * a) {
    int i = *((int *) a);    
    while(1){
        sleep(1);
        //Procedimentos para acessar a corda

        pthread_mutex_lock(&lock_ga);                              // lock para inserir turnos para travessia de gorilas

            pthread_mutex_lock(&lock_veza);         // Lock para que o programa só incremente um macaco de cada vez
                mab++;
                if(mab == 1){
                    pthread_mutex_lock(&lock_travessia);    // Lock para acesso da travessia, apenas um macaco de cada vez
                }
            pthread_mutex_unlock(&lock_veza);
        
        pthread_mutex_unlock(&lock_ga);


        printf("Macaco %d passado de A para B \n",i);
        sleep(1);

	    //Procedimentos para quando sair da corda

        pthread_mutex_lock(&lock_veza);         // Lock para a decrementacao de um macaco por vez
		    mab--;
		    if(mab == 0){ 
		        pthread_mutex_unlock(&lock_travessia);  // Unlock para liberar a travessia para outro macaco ou gorila
		    }
        pthread_mutex_unlock(&lock_veza);

    }
    pthread_exit(0);
}

void * macacoBA(void * a) {
    int i = *((int *) a);
    while(1){   
        sleep(1);
        //Procedimentos para acessar a corda
        pthread_mutex_lock(&lock_ga);  // Lock para inserir turnos para a travessia de gorilas

            pthread_mutex_lock(&lock_vezb);     // Lock para que o programa só incremente um macaco de cada vez
                mba++;
                if(mba == 1){
                    pthread_mutex_lock(&lock_travessia);    // Lock para acesso da travessia, apenas um macaco de cada vez
                }
            pthread_mutex_unlock(&lock_vezb);

        pthread_mutex_unlock(&lock_ga);
        
	    printf("Macaco %d passado de B para A \n", i);
	    sleep(1);

	    //Procedimentos para quando sair da corda

        pthread_mutex_lock(&lock_vezb);             // Lock para a decrementacao de um macaco por vez
		    mba--;
		    if(mba == 0){ 
		        pthread_mutex_unlock(&lock_travessia);  // Lock para acesso da travessia, apenas um macaco de cada vez
		    }
        pthread_mutex_unlock(&lock_vezb);

    }
    pthread_exit(0);
}

void * gorila(void * a){
    while(1){
        sleep(5);
	    //Procedimentos para acessar a corda
            pthread_mutex_lock(&lock_ga);               // Lock para garantir que nao tem ngm na corda na vez do gorila
                pthread_mutex_lock(&lock_travessia);    // Lock para ocupar a corda

	                printf("Gorila passado de A para B \n");
	                sleep(5);        

                pthread_mutex_unlock(&lock_travessia);  // Unlock para liberar a corda
            pthread_mutex_unlock(&lock_ga);

    }
    pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
            if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
                printf("Não pode criar a thread %d\n", i);
                return -1;
            }
        }else{
            if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
                printf("Não pode criar a thread %d\n", i);
                return -1;
            }
        }
    }

    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);
    
    pthread_join(macacos[0], NULL);
    return 0;
}