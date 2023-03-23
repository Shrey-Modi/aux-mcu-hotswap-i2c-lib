#ifndef __WR_I2C_H_
#define __WR_I2C_H_

#include <stdbool.h>
#include <stdint.h>

void InitI2C(uint32_t slave_addr);
void I2CSendString(uint32_t slave_addr, char array[]);
void I2CRecieveString(uint32_t slave_addr, uint32_t reg_addr, char *array);


#endif