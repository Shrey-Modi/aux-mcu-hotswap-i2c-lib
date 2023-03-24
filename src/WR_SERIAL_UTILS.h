#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "WR_HOTSWAP.h"

#ifndef __WR_SERIAL_UTILS_H_
#define __WR_SERIAL_UTILS_H_



void InitConsole(void);
void UARTPrintOut(const uint8_t *pui8Buffer);
void itoa(char *array, int16_t i);
void setupUARTInterrupt();


#endif