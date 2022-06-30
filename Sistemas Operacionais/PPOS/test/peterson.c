#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define NUM_THREADS  2
#define NUM_ELEMENTS 10

typedef struct fila {
    struct fila *prev, *next;
    int id;
} fila_t;

fila_t *queue;
int flag[1];
int turn = 0;

void print_elem(void *ptr) {
    fila_t *elem = ptr;

    if (elem) { printf(" %d ", elem->id); }
}

void *threadBody(void *id) {
    fila_t *new;
    int old;

    while (1) {
        flag[(long int) id] = 1;
        turn = (long int) id;
        // Ocupado
        while (flag[(long int) id] == 1 && turn == (long int) id);
        // Parte crítica
        old = queue->id;
        // Remove o primeiro elemento da fila
        queue_remove((queue_t **) &queue, (queue_t *) queue);
        // Gera novo numero
        new = malloc(sizeof(fila_t));
        new->id = rand() % 100;
        // Insere novo elemento no fim da fila
        queue_append((queue_t **) &queue, (queue_t *) new);
        // mostra a fila
        printf("thread %ld: tira %d, põe %d: ", (long int) id, old, new->id);
        queue_print("Fila", (queue_t *) queue, print_elem);
        // Fim da parte crítica
        flag[(long int) !id] = 0;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    long i, status;
    fila_t *node;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Carrega a fila
    for (int j = 0; j < NUM_ELEMENTS; ++j) {
        node = malloc(sizeof(fila_t));
        node->id = rand() % 100;
        queue_append((queue_t **) &queue, (queue_t *) node);
    }

    // Cria as threads
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_create(&thread[i], &attr, threadBody, (void *) i);
        if (status) {
            perror("pthread_create");
            exit(1);
        }
    }

    // Aguarda todas as threads terminarem
    for (i = 0; i < NUM_THREADS; i++) {
        status = pthread_join(thread[i], NULL);
        if (status) {
            perror("pthread_join");
            exit(1);
        }
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}