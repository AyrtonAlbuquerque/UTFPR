#include <Arduino.h>
#include <stdint.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/sysctl.h>

/* -------------------------------------------------------------------------- */
/*                                   Globals                                  */
/* -------------------------------------------------------------------------- */
uint32_t Clock; // Clock real do sistema, pagina 491 do manual da TivaWare
uint8_t Led = 0;

/* -------------------------------------------------------------------------- */
/*                                Declarations                                */
/* -------------------------------------------------------------------------- */
void Wait(uint32_t milliseconds);

void SetClock(uint32_t frequency);

/* -------------------------------------------------------------------------- */
/*                                    Main                                    */
/* -------------------------------------------------------------------------- */
void setup() {
    SetClock(24000000);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

    while (1) {
        Led ^= GPIO_PIN_0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, Led);
        Wait(1000);
    }
}

void loop() {}

/* -------------------------------------------------------------------------- */
/*                               Implementation                               */
/* -------------------------------------------------------------------------- */
void Wait(uint32_t milliseconds) {
    uint32_t i = ((milliseconds * (Clock / 1000)) / 3) - 9;

    while (i > 0) {
        i = i - 1;
    }
}

void SetClock(uint32_t frequency) {
    Clock = SysCtlClockFreqSet(SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480, frequency);
}