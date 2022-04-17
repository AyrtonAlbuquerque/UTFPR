#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "ppos.h"
#include "ppos_data.h"

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
int task_index = 1;
int quantum;
task_t main_task, dispatcher_task;
task_t *current_task;
task_t *ready, *executing, *suspended, *terminated;
struct sigaction action;
struct itimerval timer;


/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */
task_t *scheduler();

void dispatcher();

void timer_handler(int signum);

/* -------------------------------------------------------------------------- */
/*                               Implementation                               */
/* -------------------------------------------------------------------------- */
void ppos_init() {
    // Setup sigaction
    action.sa_handler = timer_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    // sigaction
    if (sigaction(SIGALRM, &action, 0) < 0) {
        perror("[PPOS-CORE|SIGACTION]: Error while setting up sigaction.");
        exit(1);
    }

    // Setup timer
    timer.it_value.tv_usec = TICKS;
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = TICKS;
    timer.it_interval.tv_sec = 0;

    // setitimer
    if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
        perror("[PPOS-CORE|ITIMERVAL]: Error while setting up timer.");
        exit(1);
    }

    // Turn off stdout buffer
    setvbuf(stdout, 0, _IONBF, 0);

    // Setup main task
    main_task.id = 0;
    main_task.next = NULL;
    main_task.prev = NULL;
    main_task.prio = main_task.d_prio = DEFAULT_PRIO;

    // Save main task context
    getcontext(&(main_task.context));

    // Set current task to main task and update its state
    current_task = &main_task;
    current_task->status = TASK_EXECUTING;

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
        task->status = TASK_NEW;
        task->prio = task->d_prio = DEFAULT_PRIO;
        task->preemptable = 0;

        // If not the dispatcher task, insert in the ready queue
        if (task != &dispatcher_task) {
            task->status = TASK_READY;
            task->preemptable = 1;
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
        current_task->status = TASK_TERMINATED;
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
        current_task->status = TASK_EXECUTING;
        // Update the quantum
        quantum = QUANTUM;

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
    current_task->status = TASK_READY;
    // Switch to dispatcher task
    task_switch(&dispatcher_task);
}

task_t *scheduler() {
    task_t *task = ready;

    // If there are tasks in the ready queue
    if (queue_size((queue_t *) ready) > 0) {
        // For each task in the ready queue
        for (task_t *node = task->next; node != ready; node = node->next) {
            // If node prio is < than task prio, node has priority over task
            if (node->d_prio < task->d_prio) {
                // Apply aging without breaking lower bound
                task->d_prio = (task->d_prio > LOWEST_PRIO) ? task->d_prio += AGING_ALPHA : LOWEST_PRIO;
                // Update chosen task
                task = node;
            } else {
                // task has priority over node, just apply aging to node
                node->d_prio = (node->d_prio > LOWEST_PRIO) ? node->d_prio += AGING_ALPHA : LOWEST_PRIO;
            }
        }
        // Reset dynamic priority for the chosen task to its static priority
        task->d_prio = task->prio;

        // Return chosen task
        return task;
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
            switch (task->status) {
                case TASK_READY:
                    queue_append((queue_t **) &ready, (queue_t *) task);
                    break;
                case TASK_EXECUTING:
                    queue_append((queue_t **) &executing, (queue_t *) task);
                    break;
                case TASK_SUSPENDED:
                    queue_append((queue_t **) &suspended, (queue_t *) task);
                    break;
                case TASK_TERMINATED:
                    queue_append((queue_t **) &terminated, (queue_t *) task);
                    break;
            }
        } else {
            break;
        }
    }
    task_exit(0);
}

void task_setprio(task_t *task, int prio) {
    // If task is null than current task
    task = !task ? current_task : task;

    // If prio is lower than -20 than set lo lower bound. If prio is greater than 20 than set to 20
    task->prio = (prio < LOWEST_PRIO) ? LOWEST_PRIO : ((prio > HIGHEST_PRIO) ? HIGHEST_PRIO : prio);
    task->d_prio = task->prio;
}

int task_getprio(task_t *task) {
    // If task is null, than current task
    task = !task ? current_task : task;

    return task->prio;
}

void timer_handler(int signum) {
    // If preemptable task (not system task) and is a timer interruption
    if (current_task->preemptable && signum == SIGALRM) {
        // If quantum is 0 yield, else decrement
        if (quantum == 0) {
            task_yield();
        } else {
            quantum--;
        }
    }
}