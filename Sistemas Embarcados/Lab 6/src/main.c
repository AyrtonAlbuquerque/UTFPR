#include "system_TM4C1294.h"
#include "driverleds.h"
#include "cmsis_os2.h"
#include "driverbuttons.h"

/* -------------------------------------------------------------------------- */
/*                                  Constants                                 */
/* -------------------------------------------------------------------------- */
#define LED_COUNT 4
#define PWM_MAX 10
#define QUEUE_SIZE 16
#define BLINK_TIME 100

const uint8_t LED[] = {LED1, LED2, LED3, LED4};

/* -------------------------------------------------------------------------- */
/*                                   Structs                                  */
/* -------------------------------------------------------------------------- */
typedef struct {
    uint8_t pwm;
    uint8_t blink;
} Message;

typedef struct TASK_STRUCT {
    uint8_t led;
    uint8_t state;
    uint8_t blink;
    osMessageQueueId_t queue;

    // método para trocar o estado do led
    void (*toggle)(struct TASK_STRUCT *this);
} Task;

typedef struct {
    osThreadId_t id;
    Task task;
} Thread;

typedef struct {
    uint32_t usw1_tick, usw2_tick;
    osMutexId_t lock;
    osMessageQueueId_t queue;
    Thread controller;
    Thread thread[LED_COUNT];
} Process;

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
Process process;

/* -------------------------------------------------------------------------- */
/*                            Method Implementation                           */
/* -------------------------------------------------------------------------- */
void Toggle(Task *this) {
    osMutexAcquire(process.lock, osWaitForever);
    this->state ^= this->led;
    LEDWrite(this->led, this->state);
    osMutexRelease(process.lock);
}

/* -------------------------------------------------------------------------- */
/*                               Button Handler                               */
/* -------------------------------------------------------------------------- */
void GPIOJ_Handler() {
    uint8_t button;

    ButtonIntClear(USW1 | USW2);
    if (!ButtonRead(USW1) && osKernelGetTickCount() - process.usw1_tick >= 250) {
        button = USW1;
        if (osMessageQueuePut(process.queue, &button, NULL, 0) == osOK)
            process.usw1_tick = osKernelGetTickCount();
    }

    if (!ButtonRead(USW2) && osKernelGetTickCount() - process.usw2_tick >= 250) {
        button = USW2;
        if (osMessageQueuePut(process.queue, &button, NULL, 0) == osOK)
            process.usw2_tick = osKernelGetTickCount();
    }
}

/* -------------------------------------------------------------------------- */
/*                               Thread Callback                              */
/* -------------------------------------------------------------------------- */
void Callback(void *arg) {
    Thread *this = (Thread *) arg;
    Message message;

    while (1) {
        if (osMessageQueueGet(this->task.queue, &message, NULL, 0) == osOK)
            this->task.blink = message.blink;

        this->task.toggle(&this->task);
        osDelay(message.pwm);
        this->task.toggle(&this->task);
        osDelay(this->task.blink * BLINK_TIME + (PWM_MAX - message.pwm));
    }
}

/* -------------------------------------------------------------------------- */
/*                             Controller Callback                            */
/* -------------------------------------------------------------------------- */
void Controller(void *arg) {
    Message message[LED_COUNT];
    uint8_t button;
    uint8_t i = 0;

    while (1) {
        if (osMessageQueueGet(process.queue, &button, NULL, osWaitForever) == osOK) {
            switch (button) {
                case USW1:
                    message[i].blink = 0;
                    osMessageQueuePut(process.thread[i].task.queue, &message[i], NULL, osWaitForever);
                    i = (i + 1) % LED_COUNT;
                    message[i].blink = 1;
                    osMessageQueuePut(process.thread[i].task.queue, &message[i], NULL, osWaitForever);
                    break;
                case USW2:
                    message[i].blink = process.thread[i].task.blink;
                    message[i].pwm = (message[i].pwm + 1) % (PWM_MAX - 1);
                    osMessageQueuePut(process.thread[i].task.queue, &message[i], NULL, osWaitForever);
                    break;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                                    Main                                    */
/* -------------------------------------------------------------------------- */
int main() {
    LEDInit(LED1 | LED2 | LED3 | LED4);
    ButtonInit(USW1 | USW2);
    ButtonIntEnable(USW1 | USW2);
    osKernelInitialize();

    process.controller.id = osThreadNew(Controller, &process.controller, NULL);
    process.queue = osMessageQueueNew(QUEUE_SIZE, sizeof(uint8_t), NULL);
    process.lock = osMutexNew(NULL);
    for (int i = 0; i < LED_COUNT; i++) {
        process.thread[i].id = osThreadNew(Callback, &process.thread[i], NULL);
        process.thread[i].task.led = LED[i];
        process.thread[i].task.state = 0;
        process.thread[i].task.blink = (i == 0) ? 1 : 0;
        process.thread[i].task.toggle = Toggle;
        process.thread[i].task.queue = osMessageQueueNew(QUEUE_SIZE, sizeof(Message), NULL);
    }

    if (osKernelGetState() == osKernelReady)
        osKernelStart();

    while (1);
}