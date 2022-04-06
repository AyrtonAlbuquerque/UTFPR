#include <stdio.h>
#include <stdlib.h>
#include "ppos.h"
#include "ppos_data.h"

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
int task_index = 1;
task_t main_task;
task_t *current_task;
queue_t *ready;

/* -------------------------------------------------------------------------- */
/*                               Implementation                               */
/* -------------------------------------------------------------------------- */
void ppos_init() {
    // Turn off stdout buffer
    setvbuf(stdout, 0, _IONBF, 0);

    // Setup main task
    main_task.id = 0;
    main_task.next = NULL;
    main_task.prev = NULL;

    // Save main task context
    getcontext(&(main_task.context));

    // Set current task to main task and update its state
    current_task = &main_task;
    current_task->state = PPOS_TASK_STATE_EXECUTING;

    // Insert main task in the ready queue
    queue_append((queue_t **) &ready, (queue_t *) &main_task);

    // Init done
#ifdef DEBUG
    printf("[PPOS-CORE|INIT]: PPOS initialization finished.");
#endif
}

int task_create(task_t *task, void (*start_func)(void *), void *arg) {
    char *stack;

    if (task) {
        // Update task state
        task->state = PPOS_TASK_STATE_NEW;

        // Save task context
        getcontext(&(task->context));

        // Allocate and configure task stack
        stack = malloc(STACKSIZE);
        if (stack) {
            task->context.uc_stack.ss_sp = stack;
            task->context.uc_stack.ss_size = STACKSIZE;
            task->context.uc_stack.ss_flags = 0;
            task->context.uc_link = 0;
        } else {
            perror("[PPOS-CORE|CREATE]: Error while allocating stack.");
            return -1;
        }

        // Make context
        makecontext(&(task->context), (void *) start_func, 1, arg);

        // If nothing went wrong, set up the task and append to ready queue
        task->id = task_index++;
        task->next = NULL;
        task->prev = NULL;
        task->state = PPOS_TASK_STATE_READY;
        queue_append((queue_t **) &ready, (queue_t *) task);

#ifdef DEBUG
        printf("[PPOS-CORE|CREATE]: Task created with id: %d\n", task->id);
#endif

        return task->id;
    } else {
        return -1;
    }
}

void task_exit(int exitCode) {
    // Update current task state and exit code
    current_task->state = PPOS_TASK_STATE_TERMINATED;
    current_task->exitCode = exitCode;

#ifdef DEBUG
    printf("[PPOS-CORE|exit]: The task %d will be terminated.\n", current_task->id);
#endif

    // Switch to main task so the queue can be processed
    task_switch(&main_task);
}

int task_switch(task_t *task) {
    task_t *aux = current_task;

    // If the task is not null and is not the current task
    if (task && task != current_task) {
        current_task->state = PPOS_TASK_STATE_SUSPENDED;        // Update current task state
        current_task = task;                                    // Set current task to the task to be switched
        current_task->state = PPOS_TASK_STATE_EXECUTING;        // Update current task state

#ifdef DEBUG
        printf("[PPOS-CORE|SWITCH]: Switching tasks %d -> %d.\n", aux->id, task->id);
#endif
        // Swap the contexts
        return swapcontext(&(aux->context), &(task->context));
    }

#ifdef DEBUG
    printf("[PPOS-CORE|SWITCH]: The task were either not initialized or is the current executing task.\n");
#endif

    return -1;
}

int task_id() {
    return current_task->id;
}