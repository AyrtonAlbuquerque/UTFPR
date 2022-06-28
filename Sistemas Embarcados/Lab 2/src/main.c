#include <stdint.h>
#include <stdbool.h>
#include <driverlib/systick.h>
#include "driverleds.h"

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
volatile uint8_t Evento = 0;

/* -------------------------------------------------------------------------- */
/*                                   Estados                                  */
/* -------------------------------------------------------------------------- */
typedef enum {
    _000,
    _001,
    _011,
    _010,
    _110,
    _111,
    _101,
    _100
} Estado;

void SysTick_Handler() {
    Evento = 1;
}

int main() {
    static Estado estado = _000;

    LEDInit(LED1 | LED2 | LED3);
    SysTickPeriodSet(12000000);
    SysTickIntEnable();
    SysTickEnable();

    while (true) {
        if (Evento) {
            Evento = 0;
            switch (estado) {
                case _000:
                    LEDOff(LED1 | LED2 | LED3);
                    estado = _001;
                    break;
                case _001:
                    LEDOff(LED1 | LED2);
                    LEDOn(LED3);
                    estado = _011;
                    break;
                case _011:
                    LEDOff(LED1);
                    LEDOn(LED2 | LED3);
                    estado = _010;
                    break;
                case _010:
                    LEDOff(LED1 | LED3);
                    LEDOn(LED2);
                    estado = _110;
                    break;
                case _110:
                    LEDOff(LED3);
                    LEDOn(LED1 | LED2);
                    estado = _111;
                    break;
                case _111:
                    LEDOn(LED1 | LED2 | LED3);
                    estado = _101;
                    break;
                case _101:
                    LEDOff(LED2);
                    LEDOn(LED1 | LED3);
                    estado = _100;
                    break;
                case _100:
                    LEDOff(LED2 | LED3);
                    LEDOn(LED1);
                    estado = _000;
                    break;
            }
        }
    }
}