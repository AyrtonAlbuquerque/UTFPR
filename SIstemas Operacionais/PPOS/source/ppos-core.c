#include <stdio.h>
#include <stdlib.h>
#include "ppos.h"
#include "ppos_data.h"

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
int task_index = 0;
task_t *main_task, *current_task;
queue_t *ready;

/* -------------------------------------------------------------------------- */
/*                               Implementation                               */
/* -------------------------------------------------------------------------- */
void ppos_init() {
    // Turn off stdout buffer
    setvbuf(stdout, 0, _IONBF, 0);

    // Create main task
    main_task = malloc(sizeof(task_t));
    main_task->id = 0;

    // Save main task context
    getcontext(&(main_task->context));

    // Set current task to main task and update its state
    current_task = main_task;
    current_task->state = PPOS_TASK_STATE_EXECUTING;

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

        // If nothing went wrong, set id, state and append to ready queue
        task->id = task_index++;
        task->state = PPOS_TASK_STATE_READY;
        queue_append(&ready, (queue_t *) task);

#ifdef DEBUG
        printf("[PPOS-CORE|CREATE]: Task created with id: %d\n", task->id);
#endif

        return task->id;
    } else {
        return -1;
    }
}

void task_exit(int exitCode) {

}

int task_switch(task_t *task) {
    return 0;
}

int task_id() {
    return 0;
}