//Ayrton Albuquerque e Vickie Costa Aranha

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue.h>

#define NUM_PROD 3
#define NUM_CONS 2
#define VAGAS 5

typedef struct fila {
    struct fila *prev, *next;
    int id;
} fila_t;

fila_t *queue;
sem_t s_vagas, s_itens, s_buffer;
int num_itens, num_vagas;

void espera(int n) {
    sleep(random() % n);                // pausa entre 0 e n segundos (inteiro)
    usleep(random() % 10000000);        // pausa entre 0 e n segundos (float))
}

//corpo de produtor
void *prodBody(void *id) {
    fila_t *new;
    long tid = (long) id;

    while (1) {
        sleep(rand() % 3);
        sem_wait(&s_vagas);
        sem_wait(&s_buffer);
        // Gera novo numero
        new = malloc(sizeof(fila_t));
        new->id = rand() % 1000;
        queue_append((queue_t **) &queue, (queue_t *) new);
        num_itens++;
        num_vagas--;
        printf("p%02ld: produziu %d\n", tid, new->id);
        sem_post(&s_buffer);
        sem_post(&s_itens);
    }
}

// corpo do consumidor
void *consBody(void *id) {
    long tid = (long) id;
    int old;

    while (1) {
        sem_wait(&s_itens);
        sem_wait(&s_buffer);
        old = queue->id;
        // Remove o primeiro elemento da fila
        queue_remove((queue_t **) &queue, (queue_t *) queue);
        num_itens--;
        num_vagas++;
        printf("                                c%02ld: consumiu %d\n", tid, old);
        sem_post(&s_buffer);
        sem_post(&s_vagas);
        sleep(rand() % 3);
    }
}

// programa principal
int main(int argc, char *argv[]) {
    pthread_t produtor[NUM_PROD];
    pthread_t consumidor[NUM_CONS];
    long i;

    num_itens = 0;
    num_vagas = VAGAS;

    //inicia os semaforos
    sem_init(&s_buffer, 0, 1);
    sem_init(&s_vagas, 0, VAGAS);
    sem_init(&s_itens, 0, 0);

    //cria produtores
    for (i = 0; i < NUM_PROD; i++) {
        if (pthread_create(&produtor[i], NULL, prodBody, (void *) i)) {
            perror("pthread_create");
            exit(1);
        }
    }

    //cria consumidores
    for (i = 0; i < NUM_CONS; i++) {
        if (pthread_create(&consumidor[i], NULL, consBody, (void *) i)) {
            perror("pthread_create");
            exit(1);
        }
    }

    // main_encerra.aqui
    pthread_exit(NULL);
}
