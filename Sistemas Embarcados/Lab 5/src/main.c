#include "system_TM4C1294.h"
#include "driverleds.h"
#include "cmsis_os2.h"
#include "driverbuttons.h"

/* -------------------------------------------------------------------------- */
/*                                 Constantes                                 */
/* -------------------------------------------------------------------------- */
#define BUFFER_SIZE 8

/* -------------------------------------------------------------------------- */
/*                                   Structs                                  */
/* -------------------------------------------------------------------------- */
typedef struct {
    osSemaphoreId_t empty, full;
} Semaphore;

typedef struct {
    uint8_t count, maxCount;
    Semaphore semaphores;
} Task;

typedef struct {
    osThreadId_t id;
    Task task;
} Thread;

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
Thread thread;
uint8_t buffer[BUFFER_SIZE];

/* -------------------------------------------------------------------------- */
/*                        Publisher/Consumer Functions                        */
/* -------------------------------------------------------------------------- */
void GPIOJ_Handler() {
    uint8_t i = thread.task.count;

    osSemaphoreAcquire(thread.task.semaphores.empty, 0);
    buffer[i % BUFFER_SIZE] = ++thread.task.count % thread.task.maxCount;
    ButtonIntClear(USW1);
    osSemaphoreRelease(thread.task.semaphores.full);
}

void Consumer(void *arg) {
    uint8_t i = 0;

    while (1) {
        osSemaphoreAcquire(thread.task.semaphores.full, osWaitForever);
        LEDWrite(LED4 | LED3 | LED2 | LED1, buffer[i++ % BUFFER_SIZE]);
        osSemaphoreRelease(thread.task.semaphores.empty);
        osDelay(200);
    }
}

/* -------------------------------------------------------------------------- */
/*                                    Main                                    */
/* -------------------------------------------------------------------------- */
int main() {
    LEDInit(LED4 | LED3 | LED2 | LED1);
    ButtonInit(USW1);
    ButtonIntEnable(USW1);
    osKernelInitialize();

    thread.id = osThreadNew(Consumer, NULL, NULL);
    thread.task.semaphores.empty = osSemaphoreNew(BUFFER_SIZE, BUFFER_SIZE, NULL);
    thread.task.semaphores.full = osSemaphoreNew(BUFFER_SIZE, 0, NULL);
    thread.task.count = 0;
    thread.task.maxCount = 16;

    if (osKernelGetState() == osKernelReady)
        osKernelStart();

    while (1);
}