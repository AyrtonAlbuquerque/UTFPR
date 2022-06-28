#include "system_TM4C1294.h"
#include "driverleds.h"
#include "cmsis_os2.h"

/* -------------------------------------------------------------------------- */
/*                          Numero de Leds utilizados                         */
/* -------------------------------------------------------------------------- */
#define LED_COUNT 4

/* -------------------------------------------------------------------------- */
/*                             Estrutura da Tarefa                            */
/* -------------------------------------------------------------------------- */
typedef struct TASK_STRUCT {
    uint32_t period;
    uint8_t led;
    uint8_t state;

    // pointer para a funcao Toggle (method).
    void (*onToggle)(struct TASK_STRUCT *this);
} Task;

/* -------------------------------------------------------------------------- */
/*                             Estrutura da Thread                            */
/* -------------------------------------------------------------------------- */
typedef struct {
    osThreadId_t id;
    Task task;
} Thread;

/* -------------------------------------------------------------------------- */
/*                            Estrutura do Processo                           */
/* -------------------------------------------------------------------------- */
typedef struct {
    Thread thread[LED_COUNT];
} Process;

/* -------------------------------------------------------------------------- */
/*                            Troca o estado do Led                           */
/* -------------------------------------------------------------------------- */
void Toggle(Task *this) {
    this->state ^= this->led;
    LEDWrite(this->led, this->state);
}

/* -------------------------------------------------------------------------- */
/*                               Thread Callback                              */
/* -------------------------------------------------------------------------- */
void Callback(void *arg) {
    Thread *this = (Thread *) arg;

    while (1) {
        this->task.onToggle(&this->task);
        osDelay(this->task.period);
    }
}

int main() {
    Process process;

    LEDInit(LED1 | LED2 | LED3 | LED4);
    osKernelInitialize();

    for (int i = 0; i < LED_COUNT; i++) {
        process.thread[i].id = osThreadNew(Callback, &process.thread[i], NULL);
        process.thread[i].task.state = 0;
        process.thread[i].task.onToggle = Toggle;
        switch (i) {
            case 0:
                process.thread[i].task.led = LED1;
                process.thread[i].task.period = 200;
                break;
            case 1:
                process.thread[i].task.led = LED2;
                process.thread[i].task.period = 300;
                break;
            case 2:
                process.thread[i].task.led = LED3;
                process.thread[i].task.period = 500;
                break;
            case 3:
                process.thread[i].task.led = LED4;
                process.thread[i].task.period = 700;
                break;
        }
    }

    if (osKernelGetState() == osKernelReady)
        osKernelStart();

    while (1);
}