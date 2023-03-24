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



int main()
{

    InitConsole();
    UARTPrintOut("\r\nTESTING-WR-HOTSWAP\r\n");
    InitHotswap();
    setupUARTInterrupt();

    while(1);

}
