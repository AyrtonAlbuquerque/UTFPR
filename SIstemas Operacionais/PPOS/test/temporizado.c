#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
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
int inserts = 0;
unsigned int systick = 0;
struct sigaction action;
struct itimerval timer;

void print_elem(void *ptr) {
    fila_t *elem = ptr;

    if (elem) { printf(" %d ", elem->id); }
}

void enter_cs(long int id) {
    while (turn != id);   // busy waiting
}

void leave_cs() {
    turn = (turn + 1) % NUM_THREADS;
}

unsigned int systime() {
    return systick;
}

void timer_handler(int signum) {
    // If timer interruption
    if (signum == SIGALRM) {
        // Update systick
        systick++;
    }
}

void calculateIPS() {
    float seg = ((float)systime())/1000;
    float ips = (seg == 0) ? 0 : ((float)inserts)/seg;

    ips = (ips > inserts) ? inserts : ips;
    printf("IPS: %.0f, Inserções: %d, Tempo: %.3f\n", ips, inserts, seg);
}

void *petersonBody(void *id) {
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
        // Insere novo elemento no fim da fila e incrementa o nº de inserções
        if (queue_append((queue_t **) &queue, (queue_t *) new) >= 0) {
            inserts++;
        }
        // mostra a fila
        printf("thread %ld: tira %d, põe %d: ", (long int) id, old, new->id);
        queue_print("Fila", (queue_t *) queue, print_elem);
        calculateIPS();
        // Fim da parte crítica
        flag[(long int) !id] = 0;
    }
    pthread_exit(NULL);
}

void *alternateBody(void *id) {
    fila_t *new;
    int old;

    while (1) {
        enter_cs((long int) id);
        old = queue->id;
        // Remove o primeiro elemento da fila
        queue_remove((queue_t **) &queue, (queue_t *) queue);
        // Gera novo numero
        new = malloc(sizeof(fila_t));
        new->id = rand() % 100;
        // Insere novo elemento no fim da fila
        if (queue_append((queue_t **) &queue, (queue_t *) new) >= 0) {
            inserts++;
        }
        // mostra a fila
        printf("thread %ld: tira %d, põe %d: ", (long int) id, old, new->id);
        queue_print("Fila", (queue_t *) queue, print_elem);
        calculateIPS();
        leave_cs();
    }
    pthread_exit(NULL);
}

void *noneBody(void *id) {
    fila_t *new;
    int old;

    while (1) {
        old = queue->id;
        // Remove o primeiro elemento da fila
        queue_remove((queue_t **) &queue, (queue_t *) queue);
        // Gera novo numero
        new = malloc(sizeof(fila_t));
        new->id = rand() % 100;
        // Insere novo elemento no fim da fila
        if (queue_append((queue_t **) &queue, (queue_t *) new) >= 0) {
            inserts++;
        }
        // mostra a fila
        printf("thread %ld: tira %d, põe %d: ", (long int) id, old, new->id);
        queue_print("Fila", (queue_t *) queue, print_elem);
        calculateIPS();
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;
    long i, status;
    fila_t *node;

    // Reseta systick
    systick = 0;

    // Setup sigaction
    action.sa_handler = timer_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM, &action, 0) < 0) {
        perror("Error while setting up sigaction.");
        exit(1);
    }

    // Setup timer
    timer.it_value.tv_usec = 1000;
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = 1000;
    timer.it_interval.tv_sec = 0;

    if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
        perror("[PPOS-CORE|ITIMERVAL]: Error while setting up timer.");
        exit(1);
    }

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
        status = pthread_create(&thread[i], &attr, petersonBody, (void *) i);
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