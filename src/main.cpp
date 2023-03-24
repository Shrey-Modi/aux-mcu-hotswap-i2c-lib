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

void InitConsole(void)
{

    //
    // Enable the UART0 module.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //
    // Wait for the UART0 module to be ready.
    //
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }
    // Initialize the UART. Set the baud rate, number of data bits, turn off
    // parity, number of stop bits, and stick mode. The UART is enabled by the

    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
}

void UARTPrintOut(const uint8_t *pui8Buffer)
{
    // print chars until '\0' char is found
    while (*pui8Buffer != '\0')
    {
        UARTCharPut(UART0_BASE, *pui8Buffer);
        pui8Buffer++;
    }
}

void itoa(char *array, int16_t i)
{
    uint32_t j = 0;
    uint32_t s_space = 0;
    if (i < 0)
    {
        array[j] = '-';
        i = -i;
        j++;
        s_space++;
    }

    uint32_t rem = 0;
    uint32_t len = 0;
    uint32_t n = i;

    while (n != 0)
    {
        len++;
        n /= 10;
    }

    for (uint32_t k = 0; k < len; k++)
    {
        rem = i % 10;
        i = i / 10;
        array[len - (k + 1)+s_space] = rem + '0';
    }

    array[len+s_space] = '\0';
}

int main()
{

    char array[15];
    InitConsole();
    UARTPrintOut("\r\nTESTING-WR-HOTSWAP\r\n");
    InitHotswap();
    UARTPrintOut("\r\nreading\r\n");
    array[5]='\0';


    while(1){
        UARTPrintOut("\r\n");
        UARTPrintOut("Shunt Voltage: ");
        int16_t t = readShuntVoltage();

        itoa(array,t);


        UARTPrintOut((const uint8_t *)array);
        UARTPrintOut("\r\n");

        SysCtlDelay(1000000);
    }

}
