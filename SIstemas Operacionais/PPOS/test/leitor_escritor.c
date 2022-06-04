//Ayrton Albuquerque e Vickie Costa Aranha

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue.h>

#define NUM_ESCRITOR 2
#define NUM_LEITOR 3
#define NUM_ELEMENTS 10

typedef struct fila {
    struct fila *prev, *next;
    int id;
} fila_t;

fila_t *queue;
pthread_mutex_t mutex;
pthread_cond_t cond;

void print_elem(void *ptr) {
    fila_t *elem = ptr;

    if (elem) { printf(" %d ", elem->id); }
}

int calculaMedia() {
    fila_t *first = queue;
    int media = 0;
    int tamanho = queue_size((queue_t *) queue);

    if (tamanho == 0) {
        return 0;
    } else {
        for (fila_t *node = first->next; node != queue; node = node->next) {
            media = media + node->id;
        }

        return media/tamanho;
    }
}

//corpo de produtor
void *escritorBody(void *id) {
    fila_t *new;

    while (1) {
        pthread_mutex_lock(&mutex);
//        pthread_cond_wait(&cond, &mutex);
        // Remove o primeiro elemento da fila
        queue_remove((queue_t **) &queue, (queue_t *) queue);
        // Gera novo numero
        new = malloc(sizeof(fila_t));
        new->id = rand() % 100;
        // Insere novo elemento no fim da fila
        queue_append((queue_t **) &queue, (queue_t *) new);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

// corpo do consumidor
void *leitorBody(void *id) {
    while (1) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        queue_print("Fila", (queue_t *) queue, print_elem);
        printf("Média: %d\n", calculaMedia());
//        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

// programa principal
int main(int argc, char *argv[]) {
    pthread_t escritor[NUM_ESCRITOR];
    pthread_t leitor[NUM_LEITOR];
    long i;
    fila_t *node;

    // Mutex
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond, NULL);

    // Carrega a fila
    for (int j = 0; j < NUM_ELEMENTS; ++j) {
        node = malloc(sizeof(fila_t));
        node->id = rand() % 100;
        queue_append((queue_t **) &queue, (queue_t *) node);
    }

    //cria escritores
    for (i = 0; i < NUM_ESCRITOR; i++) {
        if (pthread_create(&escritor[i], NULL, escritorBody, (void *) i)) {
            perror("pthread_create");
            exit(1);
        }
    }

    //cria leitores
    for (i = 0; i < NUM_LEITOR; i++) {
        if (pthread_create(&leitor[i], NULL, leitorBody, (void *) i)) {
            perror("pthread_create");
            exit(1);
        }
    }

    // main_encerra.aqui
    pthread_exit(NULL);
}
