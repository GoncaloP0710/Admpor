#include "synchronization.h"

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>  // Add this line to include fcntl.h
#include <errno.h>  // Add this line to include errno.h

sem_t* semaphore_create(char* name, int value) {
    sem_t* semaforo;
    int id = getuid();
    char sem[200];
    sprintf(sem, "%d", id);
    strcat(sem, name);
    char* sem2 = sem;
    semaforo = sem_open(name, O_CREAT, 0xFFFFFFFF, value);
    if (semaforo == SEM_FAILED){
        perror("full");
        exit(6);
    }
    return semaforo;
}

/* Função que destroi o semáforo passado em argumento.
*/
void semaphore_destroy(char* name, sem_t* semaphore) {
    if (sem_close(semaphore) == -1){
        perror("semClose");
    }
    if (sem_unlink(name) == -1){
        perror("semUnlink");
    }
}

/* Função que inicia o processo de produzir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_begin(struct prodcons* pc) {
    sem_wait(pc->empty);
    sem_wait(pc->mutex);
}

/* Função que termina o processo de produzir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void produce_end(struct prodcons* pc) {
    sem_post(pc->mutex);
    sem_post(pc->full);
}

/* Função que inicia o processo de consumir, fazendo sem_wait nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_begin(struct prodcons* pc) {
    sem_wait(pc->full);
    sem_wait(pc->mutex);
}

/* Função que termina o processo de consumir, fazendo sem_post nos semáforos
* corretos da estrutura passada em argumento.
*/
void consume_end(struct prodcons* pc) {
    sem_post(pc->mutex);
    sem_post(pc->empty);
}

/* Função que faz wait a um semáforo.
*/
void semaphore_mutex_lock(sem_t* sem) {
    sem_wait(sem);
}

/* Função que faz post a um semáforo.
*/
void semaphore_mutex_unlock(sem_t* sem) {
    sem_post(sem);
}