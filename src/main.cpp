#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include <iostream>

#include "WR_HOTSWAP.h"
#include "WR_SERIAL_UTILS.h"

#define SHUNT_RESISTANCE 1300 //in micro ohms

int main()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //
    // Enable the GPIO pin for the LED (PF3).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

    InitConsole();
    UARTPrintOut("\r\nTESTING-WR-HOTSWAP\r\n");
    InitHotswap();
    setShuntResistance(SHUNT_RESISTANCE);
    setupUARTInterrupt();
    char array[3];
//    I2CRecieveString(SLAVE_ADDRESS, CONFIG_REG_POINTER_ADRESS, array);
    I2CRecieveString(SLAVE_ADDRESS, CALIB_REG_POINTER_ADRESS, array);

    while(1);

}
