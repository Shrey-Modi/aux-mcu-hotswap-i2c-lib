#include "WR_HOTSWAP.h"

void InitHotswap()
{
    InitI2C(SLAVE_ADDRESS);
}

void setConfigReg(char *array)
{
    char data[] = {CONFIG_REG_POINTER_ADRESS, array[0], array[1], '\0'};
    I2CSendString(SLAVE_ADDRESS, data);
}

void getConfigReg(char *array)
{
    I2CRecieveString(SLAVE_ADDRESS, CONFIG_REG_POINTER_ADRESS, array);
}

void setCalibReg(char *array)
{
    char data[] = {CALIB_REG_POINTER_ADRESS, array[0], array[1], '\0'};
    I2CSendString(SLAVE_ADDRESS, data);
}

void getShuntReg(char *array)
{
    I2CRecieveString(SLAVE_ADDRESS, SHUNT_REG_POINTER_ADRESS, array);
}

void getBusReg(char *array)
{
    I2CRecieveString(SLAVE_ADDRESS, BUS_REG_POINTER_ADRESS, array);
}

void getPowerReg(char *array)
{
    I2CRecieveString(SLAVE_ADDRESS, POWER_REG_POINTER_ADRESS, array);
}

void getCurrentReg(char *array)
{
    I2CRecieveString(SLAVE_ADDRESS, CURRENT_REG_POINTER_ADRESS, array);
}

/**
 * @brief used to set the PGA gain of the shunt voltage register
 *
 * @param gain input values : 0,1,2,3 for setting gain to 1, /2, /4, /8
 *              respectively with ranges +-40mV, +-80mV, +-160mV, +-320mV
 *
 */
void setShuntVoltageGain(uint32_t gain)
{
    char array[2];
    getConfigReg(array);

    // TODO: add logic to set pga bits
    array[0] = array[0] & 0b11100111;//clearing pga bits
    array[0] = array[0] | (gain << 3);//setting pga bits

    setConfigReg(array);
}

/**
 * @brief Set the Bus Voltage ADC Res object
 * 
 * @param res 9,10,11,12 for 9,10,11,12 bit resolution respectively
 * 
 * can either set this or ADC avg but not both
 */
void setBusVoltageADCRes(uint32_t res)
{
    char array[2];
    getConfigReg(array);

    res = res-9;
    //bits 7-10 are BADC
    array[0] = array[0] & 0b11111000;//clearing BADC bits
    array[1] = array[1] & 0b01111111;//clearing BADC bits

    array[1] = array[1] | (res << 7);//setting BADC bits for res setting


    setConfigReg(array);

}

/**
 * @brief Set the Bus Voltage ADC Avg object
 * 
 * 
 * @param avg 1,2,3,4,5,6,7 for 2,4,8,16,32,64,128 samples respectively
 * 
 * can either set this or ADC res but not both
 */
void setBusVoltageADCAvg(uint32_t avg)
{
    char array[2];
    getConfigReg(array);

    //bits 7-10 are BADC
    array[0] = array[0] & 0b11111000;//clearing BADC bits
    array[1] = array[1] & 0b01111111;//clearing BADC bits

    array[0] = array[0] | 0b00000100;//setting badc4 to 1 for avg mode

    array[1] = array[1] | (avg << 7);//setting BADC bits for avg setting
    array[0] = array[0] | (avg >> 1);//setting BADC bits for avg setting

    setConfigReg(array);

}    

/**
 * @brief Set the Shunt Voltage ADC Res object
 * 
 * @param res 9,10,11,12 for 9,10,11,12 bit resolution respectively
 * 
 * can either set this or ADC avg but not both
 */
void setShuntVoltageADCRes(uint32_t res)
{
    char array[2];
    getConfigReg(array);

    res = res-9;
    //bits 3-6 are SADC
    array[1] = array[1] & 0b10000111;//clearing SADC bits

    array[1] = array[1] | (res << 3);//setting SADC bits for res setting

}

/**
 * @brief Set the Shunt Voltage ADC Avg object
 * 
 * 
 * @param avg 1,2,3,4,5,6,7 for 2,4,8,16,32,64,128 samples respectively
 * 
 * can either set this or ADC res but not both
 */
void setShuntVoltageADCAvg(uint32_t avg)
{
    char array[2];
    getConfigReg(array);

    //bits 3-6 are SADC
    array[1] = array[1] & 0b10000111;//clearing SADC bits

    array[1] = array[1] | 0b01000000;//setting sadc4 to 1 for avg mode

    array[1] = array[1] | (avg << 3);//setting SADC bits for avg setting

    setConfigReg(array);

}

/**
 * @brief reads the shunt voltage register and returns the value in 2s complement 2 byte integer
 * 
 * @return int16_t voltage in mV multiplied by 100
 */
int16_t readShuntVoltage()
{
    char array[2];
    getShuntReg(array);

    int16_t voltage = (array[0] << 8) | array[1];    

    return voltage;
}

/**
 * @brief reads the bus voltage register and returns the value in 2s complement 2 byte integer
 * 
 * @return int16_t bus voltage in mV
 */
int16_t readBusVoltage()
{
    char array[2];
    getBusReg(array);

    int16_t voltage = (array[0] << 5) | (array[1] >> 3);    

    return voltage*4;
}

/**
 * @brief Set the Shunt Resistance in the calibration register
 * 
 * @param resistance in micro ohms
 */
void setShuntResistance(uint16_t resistance)
{
    char array[2];
//    figure out LSB term which will be between maxI/32767 and maxI/4096
    // maxI = 30A
    //
    uint16_t calib = 40960 /(LSB * resistance); // = 4096000000 / (resistance * LSB)
    //should be 0.04096 / ((40/32767) * (1300/(10^6))) = 40960 / ((40/32767) * 1300) = 25810

//    uint16_t calib = 25810;
    array[0] = calib >> 8;
    array[1] = calib & 0xFF;
    setCalibReg(array);
}

uint32_t readCurrent()
{
    char array[2];
    getCurrentReg(array);

    uint32_t current = (array[0] << 8) | array[1];    

    return current;

}

/**
 * @brief reads the power register
 * 
 * @return uint32_t current in watts
 */
uint32_t readPower()
{
    char array[2];
    getPowerReg(array);

    uint32_t power = (array[0] << 8) | array[1];    

    return power;

}
