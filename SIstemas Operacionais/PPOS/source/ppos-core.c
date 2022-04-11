#include <stdio.h>
#include <stdlib.h>
#include "ppos.h"
#include "ppos_data.h"

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
int task_index = 1;
task_t main_task, dispatcher_task;
task_t *current_task;
// Queues
task_t *ready, *executing, *suspended, *terminated;

/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */
task_t *scheduler();

void dispatcher();

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

    // Create dispatcher task with task_create
    task_create(&dispatcher_task, (void *) dispatcher, NULL);

    // Init done
#ifdef DEBUG
    printf("[PPOS-CORE|INIT]: PPOS initialization finished.");
#endif
}

int task_create(task_t *task, void (*start_func)(void *), void *arg) {
    char *stack;

    if (task) {
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

        // If nothing went wrong, set up the task
        task->id = task_index++;
        task->next = NULL;
        task->prev = NULL;
        task->state = PPOS_TASK_STATE_NEW;

        // If not the dispatcher task, insert in the ready queue
        if (task != &dispatcher_task) {
            task->state = PPOS_TASK_STATE_READY;
            queue_append((queue_t **) &ready, (queue_t *) task);
        }

#ifdef DEBUG
        printf("[PPOS-CORE|CREATE]: Task created with id: %d\n", task->id);
#endif

        return task->id;
    } else {
        return -1;
    }
}

void task_exit(int exitCode) {
    // If the current task is the dispatcher task, switch to main, else switch to dispatcher
    if (current_task == &dispatcher_task) {
        task_switch(&main_task);
    } else {
        // Update current task state and exit code
        current_task->state = PPOS_TASK_STATE_TERMINATED;
        current_task->exitCode = exitCode;

#ifdef DEBUG
        printf("[PPOS-CORE|EXIT]: The task %d will be terminated.\n", current_task->id);
#endif
        task_switch(&dispatcher_task);
    }
}

int task_switch(task_t *task) {
    task_t *aux = current_task;

    // If the task is not null and is not the current task
    if (task != current_task) {
        // Set current task to the task to be switched and update its state
        current_task = task;
        current_task->state = PPOS_TASK_STATE_EXECUTING;

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

void task_yield() {
    // Update current task state
    current_task->state = PPOS_TASK_STATE_READY;
    // Switch to dispatcher task
    task_switch(&dispatcher_task);
}

task_t *scheduler() {
    // If there are tasks in the ready queue
    if (queue_size((queue_t *) ready) > 0) {
        return ready;   //FCFS
    }
    return NULL;
}

void dispatcher() {
    task_t *task;

    // Dispatcher runs in a loop
    while (1) {
        // Get the task from the scheduler
        task = scheduler();

        // if valid task
        if (task) {
            // Remove the task from the ready queue and switch to it
            queue_remove((queue_t **) &ready, (queue_t *) task);
            task_switch(task);

            // Insert the task in the proper queue
            switch (task->state) {
                case PPOS_TASK_STATE_READY:
                    queue_append((queue_t **) &ready, (queue_t *) task);
                    break;
                case PPOS_TASK_STATE_EXECUTING:
                    queue_append((queue_t **) &executing, (queue_t *) task);
                    break;
                case PPOS_TASK_STATE_SUSPENDED:
                    queue_append((queue_t **) &suspended, (queue_t *) task);
                    break;
                case PPOS_TASK_STATE_TERMINATED:
                    queue_append((queue_t **) &terminated, (queue_t *) task);
                    break;
            }
        } else {
            break;
        }
    }
    task_exit(0);
}
