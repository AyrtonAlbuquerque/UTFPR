// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.4 -- Janeiro de 2022

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <ucontext.h>                   // biblioteca POSIX de trocas de contexto
#include "queue.h"                      // biblioeca de filas

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t {
    struct task_t *prev, *next;         // ponteiros para usar em filas
    int id;                             // identificador da tarefa
    ucontext_t context;                 // contexto armazenado da tarefa
    short status;                       // pronta, rodando, suspensa, ...
    short preemptable;                  // pode ser preemptada?
    int exitCode;                       // Código de sáida da tarefa
    int prio, d_prio;                   // prioridades estática e dinâmica
    // ... (outros campos serão adicionados mais tarde)
} task_t;

// estrutura que define um semáforo
typedef struct {
    // preencher quando necessário
} semaphore_t;

// estrutura que define um mutex
typedef struct {
    // preencher quando necessário
} mutex_t;

// estrutura que define uma barreira
typedef struct {
    // preencher quando necessário
} barrier_t;

// estrutura que define uma fila de mensagens
typedef struct {
    // preencher quando necessário
} mqueue_t;

/* -------------------------------------------------------------------------- */
/*                                   Defines                                  */
/* -------------------------------------------------------------------------- */

/* ------------------------------- Task Status ------------------------------ */
#define TASK_NEW            0
#define TASK_READY          1
#define TASK_EXECUTING      2
#define TASK_SUSPENDED      3
#define TASK_TERMINATED     4

/* -------------------------------- Stacksize ------------------------------- */
#define STACKSIZE           32768

/* ------------------------------- Priorities ------------------------------- */
#define DEFAULT_PRIO        0
#define HIGHEST_PRIO        20
#define LOWEST_PRIO         -20
#define AGING_ALPHA         -1

#endif

