#include <stdio.h>
#include "queue.h"

int queue_append(queue_t **queue, queue_t *elem) {
    if (!queue) { return -1; }
    if (!elem) { return -1; }
    if (elem->next || elem->prev) { return -1; }

    if (!*queue) {
        *queue = elem;
        elem->next = elem;
        elem->prev = elem;
    } else {
        elem->next = *queue;
        elem->prev = (*queue)->prev;
        (*queue)->prev->next = elem;
        (*queue)->prev = elem;
    }

    return 0;
}

int queue_remove(queue_t **queue, queue_t *elem) {
    if (!queue) { return -1; }
    if (!elem) { return -1; }
    if (!*queue) { return -1; }

    queue_t *node = *queue;
    do {
        if (node == elem) {
            *queue = (node == *queue) ? (node->next == *queue ? NULL : node->next) : *queue;
            node->next->prev = node->prev;
            node->prev->next = node->next;
            node->next = NULL;
            node->prev = NULL;
            return 0;
        }
        node = node->next;
    } while (node != *queue);

    return -1;
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