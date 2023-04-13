/**
 * library for wisconsin racing to use the Hotswap Controller
 *
 */

#ifndef __WR_HOTSWAP_H_
#define __WR_HOTSWAP_H_


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

#define SLAVE_ADDRESS 0x40
#define CONFIG_REG_POINTER_ADRESS 0x00
#define SHUNT_REG_POINTER_ADRESS 0x01
#define BUS_REG_POINTER_ADRESS 0x02
#define POWER_REG_POINTER_ADRESS 0x03
#define CURRENT_REG_POINTER_ADRESS 0x04
#define CALIB_REG_POINTER_ADRESS 0x05
#define LSB (float)40/32767 // taking maxI to be 40(actual was 30)

/**
 * @brief initializes the hotswap controller and the I2C bus
 *
 */
void InitHotswap();

/**
 * @brief used to set the PGA gain of the shunt voltage register
 *
 * @param gain input values : 0,1,2,3 for setting gain to 1, /2, /4, /8
 *              respectively with ranges +-40mV, +-80mV, +-160mV, +-320mV
 *
 */
void setShuntVoltageGain(uint32_t gain);

/**
 * @brief Set the Bus Voltage ADC Avg object
 * 
 * 
 * @param avg 1,2,3,4,5,6,7 for 2,4,8,16,32,64,128 samples respectively
 * 
 * can either set this or ADC res but not both
 */
void setBusVoltageADCAvg(uint32_t avg);

/**
 * @brief Set the Shunt Voltage ADC Res object
 * 
 * @param res 9,10,11,12 for 9,10,11,12 bit resolution respectively
 * 
 * can either set this or ADC avg but not both
 */
void setShuntVoltageADCRes(uint32_t res);

/**
 * @brief Set the Shunt Voltage ADC Avg object
 * 
 * 
 * @param avg 1,2,3,4,5,6,7 for 2,4,8,16,32,64,128 samples respectively
 * 
 * can either set this or ADC res but not both
 */
void setShuntVoltageADCAvg(uint32_t avg);

/**
 * @brief reads the shunt voltage register and returns the value in 2s complement 2 byte integer
 *
 * @return int16_t voltage multiplied by 100
 */
int16_t readShuntVoltage();

/**
 * @brief reads the bus voltage register and returns the value in 2s complement 2 byte integer
 *
 * @return int16_t
 */
int16_t readBusVoltage();

/**
 * @brief Set the Shunt Resistance in the calibration register
 * 
 * @param resistance in micro ohms
 */
void setShuntResistance(uint16_t resistance);

uint32_t readCurrent();


#endif
