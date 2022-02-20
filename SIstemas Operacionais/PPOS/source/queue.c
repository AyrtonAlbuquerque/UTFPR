#include <stdio.h>
#include "queue.h"

void queue_append(queue_t **queue, queue_t *elem) {
    if (!queue) { fprintf(stderr, "[QUEUE|APPEND]: Queue is NULL.\n"); return; }
    if (!elem) { fprintf(stderr, "[QUEUE|APPEND]: Element is NULL.\n"); return; }
    if (elem->next || elem->prev) { fprintf(stderr, "[QUEUE|APPEND]: Element belongs to another queue.\n"); return; }

    if (!*queue) {
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
    }
    else {
        elem->next = *queue;
        elem->prev = (*queue)->prev;
        (*queue)->prev->next = elem;
        (*queue)->prev = elem;
    }
}

queue_t *queue_remove(queue_t **queue, queue_t *elem) {
    if (!queue) { fprintf(stderr, "[QUEUE|REMOVE]: Queue is NULL.\n"); return NULL; }
    if (!elem) { fprintf(stderr, "[QUEUE|REMOVE]: Element is NULL.\n"); return NULL; }
    if (!*queue) { fprintf(stderr, "[QUEUE|REMOVE]: Queue is empty.\n"); return NULL; }

    queue_t *node = *queue;
    do {
        if (node == elem) {
            *queue = (node == *queue) ? (node->next == *queue ? NULL : node->next) : *queue;
            node->next->prev = node->prev;
            node->prev->next = node->next;
            node->next = NULL;
            node->prev = NULL;
            return node;
        }
        node = node->next;
    } while (node != *queue);

    fprintf(stderr, "[QUEUE|REMOVE]: Element does not belong to the queue.\n");
    return NULL;
}

int queue_size(queue_t *queue) {
    int size = 1;

    if (!queue) return 0;

    for (queue_t *node = queue->next; node != queue; node = node->next, size++);

    return size;
}

void queue_print(char *name, queue_t *queue, void print_elem(void *)) {
    queue_t *node = queue;

    printf("%s: [", name);
    if (queue) {
        do {
            print_elem(node);
            if (node->next != queue)
                printf(" ");
            node = node->next;
        } while (node != queue);
    }
    printf("]\n");
}