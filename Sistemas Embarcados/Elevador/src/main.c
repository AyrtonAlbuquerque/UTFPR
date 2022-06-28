#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "system_TM4C1294.h"
#include "cmsis_os2.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "uartstdio.h"

extern void UARTStdioIntHandler(void);

/* -------------------------------------------------------------------------- */
/*                                  Constants                                 */
/* -------------------------------------------------------------------------- */
#define ELEVATOR_COUNT 3
#define QUEUE_SIZE 8
#define MESSAGE_SIZE 8
#define NUMBER_OF_FLOORS 15
#define WAITING_PERIOD 5000

/* -------------------------------------------------------------------------- */
/*                                   Structs                                  */
/* -------------------------------------------------------------------------- */
typedef struct Message_t {
    char string[MESSAGE_SIZE];
} Message;

typedef struct Task_t {
    char *self;
    uint32_t position;
    uint8_t ready, opened, floor, destination, moving;
    uint8_t flag[NUMBER_OF_FLOORS];
    osMessageQueueId_t queue, next;

    // methods
    void (*write)(Message message);

    void (*close)(struct Task_t *this);

    void (*open)(struct Task_t *this);

    void (*start)(struct Task_t *this);

    void (*stop)(struct Task_t *this);

    void (*buttonOn)(struct Task_t *this, char *button);

    void (*buttonOff)(struct Task_t *this, char *button);

    void (*goToFloor)(struct Task_t *this, uint8_t floor);
} Task;

typedef struct {
    osThreadId_t id;
    Task task;
} Thread;

typedef struct {
    osMessageQueueId_t queue;
    Thread controller;
    Thread thread[ELEVATOR_COUNT];
} Process;

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
Process process;

/* -------------------------------------------------------------------------- */
/*                                    UART                                    */
/* -------------------------------------------------------------------------- */
void UART0_Handler(void) {
    UARTStdioIntHandler();
}

void UARTInit(void) {
    // Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, SystemCoreClock);

    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void UART_Write(void *arg) {
    Message message;

    while (1) {
        if (osMessageQueueGet(process.queue, &message, NULL, osWaitForever) == osOK)
            UARTprintf(message.string);
    }
}

/* -------------------------------------------------------------------------- */
/*                            Method Implementation                           */
/* -------------------------------------------------------------------------- */
void Write(Message message) {
    strcat(message.string, "\n\r");
    osMessageQueuePut(process.queue, &message, NULL, 0);
}

void Close(Task *this) {
    Message request = {'\0'};

    strcat(request.string, this->self);
    strcat(request.string, "f");
    this->write(request);
}

void Open(Task *this) {
    Message request = {'\0'};

    if (!this->opened) {
        strcat(request.string, this->self);
        strcat(request.string, "a");
        this->flag[this->floor] = 0;
        this->write(request);
    }
}

void Start(Task *this) {
    Message request = {'\0'};

    strcat(request.string, this->self);
    strcat(request.string, "r");
    this->ready = 1;
    this->write(request);
}

void Stop(Task *this) {
    Message request = {'\0'};

    if (this->moving) {
        strcat(request.string, this->self);
        strcat(request.string, "p");
        this->moving = 0;
        this->write(request);
    }
}

void GoTo(Task *this, uint8_t floor) {
    uint32_t tick;
    Message request = {'\0'};

    if (this->moving) {
        if (!this->flag[floor]) {
            if (osMessageQueuePut(this->next, &floor, NULL, 0) == osOK)
                this->flag[floor] = 1;
        }
        return;
    }

    if (floor == this->floor)
        return;

    tick = osKernelGetTickCount();
    this->destination = floor;
    this->moving = 1;
    this->close(this);
    strcat(request.string, this->self);

    if (floor > this->floor)
        strcat(request.string, "s");
    else
        strcat(request.string, "d");

    osDelayUntil(tick + 400);
    this->write(request);
}

void ButtonOn(Task *this, char *button) {
    Message request = {'\0'};

    if (this->floor != this->destination) {
        strcat(request.string, this->self);
        strcat(request.string, "L");
        strcat(request.string, button);
        this->write(request);
    }
}

void ButtonOff(Task *this, char *button) {
    Message request = {'\0'};

    strcat(request.string, this->self);
    strcat(request.string, "D");
    strcat(request.string, button);
    this->write(request);
}

/* -------------------------------------------------------------------------- */
/*                              Elevator Callback                             */
/* -------------------------------------------------------------------------- */
void Elevator(void *arg) {
    Thread *this = (Thread *) arg;
    Message request = {'\0'};
    char *FLOOR[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p"};

    while (1) {
        if (!this->task.ready)
            this->task.start(&this->task);

        if (this->task.moving) { // Se o elevador está se movendo
            if (this->task.floor == this->task.destination) { // Checa se o andar atual é o andar de destino
                this->task.stop(&this->task); // Para o elevador
                this->task.buttonOff(&this->task, FLOOR[this->task.floor]); // Desliga o botão interno
                this->task.open(&this->task); // Abre a porta
                osDelay(WAITING_PERIOD); // Aguarda um tempo fixo
                if (osMessageQueueGet(this->task.next, &this->task.destination, NULL, 0) == osOK) { // Se existem andares a serem atendidos
                    this->task.goToFloor(&this->task, this->task.destination); // Seta o novo destino do elevador
                }
            }
        }

        if (osMessageQueueGet(this->task.queue, &request, NULL, 0) == osOK) { // Se há requests
            switch (request.string[1]) {
                case 'A':
                    this->task.opened = 1; // Caso o bit 1 da mensagem de request seja 'A', então a porta foi aberta
                    break;
                case 'F':
                    this->task.opened = 0; // Caso o bit 1 da mensagem de request seja 'F', então a porta foi fechada
                    break;
                case 'E': // Caso o bit 1 da mensagem de request seja 'E', então o houve uma requisição do elevador no andar representado pelos bit 2 e 3 da mensagem
                    this->task.goToFloor(&this->task, 10 * (request.string[2] - '0') + (request.string[3] - '0')); // Seta o novo destino do elevador
                    break;
                case 'I': // Caso o bit 1 da mensagem de request seja 'I', então foi requerido um andar de dentro do elevador
                    this->task.goToFloor(&this->task, request.string[2] - 'a'); // Seta o novo destino do elevador
                    this->task.buttonOn(&this->task, FLOOR[request.string[2] - 'a']); // Liga o botão interno
                    break;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                             Controller Callback                            */
/* -------------------------------------------------------------------------- */
void Controller(void *arg) {
    int i;
    Message message;

    while (1) {
        UARTgets(message.string, 8);

        i = ((message.string[0] == 'e') ? 0 : ((message.string[0] == 'c') ? 1 : ((message.string[0] == 'd') ? 2 : -1)));
        if (i != -1) { // Se é uma mensagem vinda de um dos elevadores
            if ((message.string[1] - '0') <= 9) // Se o bit 1 da mensagem for  um número, então a mensagem é uma notificação do sensor de andar
                process.thread[i].task.floor = (message.string[2] == '\0') ? (message.string[1] - '0') : (10 * (message.string[1] - '0') + (message.string[2] - '0'));
            else // Caso contrário é um request
                osMessageQueuePut(process.thread[i].task.queue, &message, NULL, 0); // Coloca na fila do elevador requerido
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                                    Main                                    */
/* -------------------------------------------------------------------------- */
int main() {
    UARTInit();
    osKernelInitialize();
    osThreadNew(UART_Write, NULL, NULL);

    process.controller.id = osThreadNew(Controller, &process.controller, NULL);
    process.queue = osMessageQueueNew(QUEUE_SIZE, sizeof(Message), NULL);
    for (int i = 0; i < ELEVATOR_COUNT; i++) {
        process.thread[i].task.ready = 0;
        process.thread[i].task.opened = 0;
        process.thread[i].task.floor = 0;
        process.thread[i].task.moving = 0;
        process.thread[i].task.position = 0;
        process.thread[i].task.destination = 0;
        process.thread[i].task.write = Write;
        process.thread[i].task.close = Close;
        process.thread[i].task.open = Open;
        process.thread[i].task.start = Start;
        process.thread[i].task.stop = Stop;
        process.thread[i].task.buttonOn = ButtonOn;
        process.thread[i].task.buttonOff = ButtonOff;
        process.thread[i].task.goToFloor = GoTo;
        process.thread[i].task.self = ((i == 0) ? "e" : ((i == 1) ? "c" : "d"));
        process.thread[i].id = osThreadNew(Elevator, &process.thread[i], NULL);
        process.thread[i].task.queue = osMessageQueueNew(QUEUE_SIZE, sizeof(Message), NULL);
        process.thread[i].task.next = osMessageQueueNew(NUMBER_OF_FLOORS + 1, sizeof(uint8_t), NULL);
        for (int j = 0; j < NUMBER_OF_FLOORS; j++) {
            process.thread[i].task.flag[j] = 0;
        }
    }

    if (osKernelGetState() == osKernelReady)
        osKernelStart();

    while (1);
}