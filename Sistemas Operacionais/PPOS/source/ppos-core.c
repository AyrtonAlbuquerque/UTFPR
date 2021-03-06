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
task_t *ready, *executing, *suspended, *terminated, *sleeping;
struct sigaction action;
struct itimerval timer;
unsigned int systick;


/* -------------------------------------------------------------------------- */
/*                                  Functions                                 */
/* -------------------------------------------------------------------------- */
task_t *scheduler();

void dispatcher();

void timer_handler(int signum);

unsigned int systime();

/* -------------------------------------------------------------------------- */
/*                               Implementation                               */
/* -------------------------------------------------------------------------- */
void ppos_init() {
    // Reset systick
    systick = 0;

    // Turn off stdout buffer
    setvbuf(stdout, 0, _IONBF, 0);

    // Setup sigaction
    action.sa_handler = timer_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM, &action, 0) < 0) {
        perror("[PPOS-CORE|SIGACTION]: Error while setting up sigaction.");
        exit(1);
    }

    // Setup timer
    timer.it_value.tv_usec = TICKS;
    timer.it_value.tv_sec = 0;
    timer.it_interval.tv_usec = TICKS;
    timer.it_interval.tv_sec = 0;

    if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
        perror("[PPOS-CORE|ITIMERVAL]: Error while setting up timer.");
        exit(1);
    }

    // Setup main task
    main_task.id = 0;
    main_task.next = NULL;
    main_task.prev = NULL;
    main_task.prio = main_task.d_prio = DEFAULT_PRIO;
    main_task.preemptable = 1;
    main_task.status = TASK_READY;
    main_task.startTime = systime();
    main_task.cpuTime = 0;
    main_task.activations = 0;

    // Save main task context
    getcontext(&(main_task.context));

    // Insert main task in the ready queue
    queue_append((queue_t **) &ready, (queue_t *) &main_task);

    // Set current task to main task
    current_task = &main_task;

    // Create dispatcher task with task_create
    task_create(&dispatcher_task, (void *) dispatcher, NULL);

    // Switch to the dispatcher
    task_switch(&dispatcher_task);

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
        task->status = TASK_READY;
        task->prio = task->d_prio = DEFAULT_PRIO;
        task->preemptable = 0;
        task->startTime = systime();
        task->cpuTime = 0;
        task->activations = 0;

        // If not the dispatcher task, insert in the ready queue and make it preemptable
        if (task != &dispatcher_task) {
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
    task_t *task;

#ifdef DEBUG
    printf("[PPOS-CORE|EXIT]: The task %d will be terminated.\n", current_task->id);
#endif

    // Display metrics
    printf("Task %d exit: execution time  %4d ms, processor time  %4d ms, %4d activations\n", current_task->id,
           systime() - current_task->startTime, current_task->cpuTime, current_task->activations);

    // If the current task is the dispatcher task, switch to main, else switch to dispatcher
    if (current_task == &dispatcher_task) {
        task_switch(&main_task);
    } else {
        // Update current task state and exit code
        current_task->status = TASK_TERMINATED;
        current_task->exitCode = exitCode;
        // Insert joined tasks in ready queue
        while (queue_size((queue_t *) current_task->joined) > 0) {
            task = current_task->joined;
            queue_remove((queue_t **) &current_task->joined, (queue_t *) current_task->joined);
            queue_append((queue_t **) &ready, (queue_t *) task);
        }
        // Insert current task in terminated queue
        queue_append((queue_t **) &terminated, (queue_t *) current_task);
        // Switch to dispatcher
        task_switch(&dispatcher_task);
    }
}

int task_switch(task_t *task) {
    task_t *aux = current_task;

    // Set current task to the task to be switched and update its state and activations
    current_task = task;
    current_task->activations++;
    current_task->status = TASK_EXECUTING;
    // Update the quantum
    quantum = QUANTUM;

#ifdef DEBUG
    printf("[PPOS-CORE|SWITCH]: Switching tasks %d -> %d.\n", aux->id, task->id);
#endif
    // Swap the contexts
    return swapcontext(&(aux->context), &(task->context));
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
    int size;

    // Dispatcher runs in a loop
    while (1) {
        size = queue_size((queue_t *) sleeping);
        // If there are sleeping tasks
        if (size > 0) {
            task = sleeping;
            // for each task in sleeping queue
            for (int i = 0; i < size; i++) {
                // Save next task in case of queue remove
                task_t *next = task->next;
                // If the task should wake
                if (task->wakeTime <= systime()) {
                    // Remove the task from sleeping queue
                    queue_remove((queue_t **) &sleeping, (queue_t *) task);
                    // Insert the task in the ready queue
                    queue_append((queue_t **) &ready, (queue_t *) task);
                }
                // Set pointer to next task
                task = next;
            }
        }

        // Get a task from the scheduler
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
            // If there are no sleeping or ready tasks, exit
            if (size == 0) { task_exit(0); }
            continue;
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

int task_join(task_t *task) {
    // If the task is not valid, return -1
    if (task) {
        // If the task has already been terminated, return exitCode
        if (task->status == TASK_TERMINATED)
            return task->exitCode;
        // Suspend and insert the current task in the task joined queue
        current_task->status = TASK_SUSPENDED;
        queue_append((queue_t **) &task->joined, (queue_t *) current_task);
        // Switch to dispatcher and return exitCode
        task_switch(&dispatcher_task);
        return task->exitCode;
    } else
        return -1;
}

void task_suspend(task_t **queue) {
    if (queue) {
        // Remove the current task from the ready queue
        queue_remove((queue_t **) &ready, (queue_t *) current_task);
        // Set current task status to suspended
        current_task->status = TASK_SUSPENDED;
        // Insert the current task in the pointed queue
        queue_append((queue_t **) queue, (queue_t *) current_task);
        // Switch to dispatcher
        task_switch(&dispatcher_task);
    }
}

void task_resume(task_t *task, task_t **queue) {
    if (task && queue) {
        // Remove task from queue
        queue_remove((queue_t **) queue, (queue_t *) task);
        // Set task status to ready
        task->status = TASK_READY;
        // Insert task in ready queue
        queue_append((queue_t **) &ready, (queue_t *) task);
    }
}

void task_sleep(int t) {
    // If sleep time is greater than 0
    if (t > 0) {
        // Set current task status to sleeping
        current_task->status = TASK_SLEEPING;
        // Set current task wake time
        current_task->wakeTime = systime() + t;
        // Remove current task from ready queue
        queue_remove((queue_t **) &ready, (queue_t *) current_task);
        // Insert current task in sleeping queue
        queue_append((queue_t **) &sleeping, (queue_t *) current_task);
        // Switch to dispatcher
        task_switch(&dispatcher_task);
    }
}

void timer_handler(int signum) {
    // If timer interruption
    if (signum == SIGALRM) {
        // Update systick
        systick++;

        // If current task is not null
        if (current_task) {
            // Update task processor time
            current_task->cpuTime++;

            // If preemptable task (not system task)
            if (current_task->preemptable) {
                // If quantum is 0 yield, else decrement
                if (quantum == 0) {
                    task_yield();
                } else {
                    quantum--;
                }
            }
        }
    }
}

unsigned int systime() {
    return systick;
}