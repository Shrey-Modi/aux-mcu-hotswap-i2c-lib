#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

include "WR_I2C.h"

void InitI2C(uint32_t slave_addr)
{

    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //
    // The I2C0 peripheral must be enabled before use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //
    // For this example I2C0 is used with PortB[3:2].  The actual port and
    // pins used may be different on your part, consult the data sheet for
    // more information.  GPIO port B needs to be enabled so these pins can
    // be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    //
    // Select the I2C function for these pins.  This function will also
    // configure the GPIO pins pins for I2C operation, setting them to
    // open-drain operation with weak pull-ups.  Consult the data sheet
    // to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
    while (I2CMasterBusy(I2C0_BASE))
        ;

    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false); // setting up for write
    while (I2CMasterBusy(I2C0_BASE))
        ;
}

void I2CSendString(uint32_t slave_addr, char array[])
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CSlaveACKOverride(I2C0_BASE, true); // check this line
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    // put data to be sent into FIFO
    I2CMasterDataPut(I2C0_BASE, array[0]);

    // if there is only one argument, we only need to use the
    // single send I2C function
    if (array[1] == '\0')
    {

        // Initiate send of data from the MCU
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until MCU is done transferring.
        while (I2CMasterBusy(I2C0_BASE))
            ;
    }

    // otherwise, we start transmission of multiple bytes on the
    // I2C bus
    else
    {
        // Initiate send of data from the MCU
        //
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        // Wait until MCU is done transferring.
        while (I2CMasterBusy(I2C0_BASE))
            ;

        // initialize index into array
        uint8_t i = 1;

        // send num_of_args-2 pieces of data, using the
        // BURST_SEND_CONT command of the I2C module
        while (array[i + 1] != '\0')
        {
            // put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C0_BASE, array[i++]);

            // send next data that was just placed into FIFO
            I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

            // Wait until MCU is done transferring.
            while (I2CMasterBusy(I2C0_BASE))
                ;
        }

        // put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C0_BASE, array[i]);

        // send next data that was just placed into FIFO
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

        // Wait until MCU is done transferring.
        while (I2CMasterBusy(I2C0_BASE))
            ;
    }
}

void I2CRecieveString(uint32_t slave_addr, uint32_t reg_addr, char *array)
{

    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false); // setting up for write
    while (I2CMasterBusy(I2C0_BASE))
        ;

    I2CMasterDataPut(I2C0_BASE, reg_addr); // sending reg pointer address for configuration register

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    while (I2CMasterBusy(I2C0_BASE))
        ;

    //
    // Modifiy the data direction to true, so that seeing the address will
    // indicate that the I2C Master is initiating a read from the slave.
    //
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, true);

    //
    // Do a dummy receive to make sure you don't get junk on the first receive.
    //
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    for (ui32Index = 0; ui32Index < 2; ui32Index++)
    {

        //
        // Read the data from the master.
        //
        while (I2CMasterBusy(I2C0_BASE))
            ;
        array[ui32Index] = I2CMasterDataGet(I2C0_BASE);

        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
    }

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

}