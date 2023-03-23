#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#include "WR_I2C.h"
#include "WR_HOTSWAP.h"

void InitHotswap()
{
    InitI2C(SLAVE_ADDRESS);
}

void setConfigReg(char[] array)
{
    I2CSendString(SLAVE_ADDRESS, CONFIG_REG_POINTER_ADRESS, array);
}

void getConfigReg(char* array)
{
    I2CRecieveString(SLAVE_ADDRESS, CONFIG_REG_POINTER_ADRESS, array);
}

void setCalibReg(char[] array)
{
    I2CSendString(SLAVE_ADDRESS, CALIB_REG_POINTER_ADRESS, array);
}

void getShuntReg(char* array)
{
    I2CRecieveString(SLAVE_ADDRESS, SHUNT_REG_POINTER_ADRESS, array);
}

void getBusReg(char* array)
{
    I2CRecieveString(SLAVE_ADDRESS, BUS_REG_POINTER_ADRESS, array);
}

void getPowerReg(char* array)
{
    I2CRecieveString(SLAVE_ADDRESS, POWER_REG_POINTER_ADRESS, array);
}

void getCurrentReg(char* array)
{
    I2CRecieveString(SLAVE_ADDRESS, CURRENT_REG_POINTER_ADRESS, array);
}

void setShuntVoltageGain(uint32_t gain)
{
    char array[2];
    getConfigReg(array);


    setConfigReg(array);
}