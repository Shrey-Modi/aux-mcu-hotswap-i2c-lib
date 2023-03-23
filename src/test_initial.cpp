

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


#define NUM_I2C_DATA 10

//*****************************************************************************
// adress for TPS2480/81 when A1 and A0 are both grounded
//*****************************************************************************
#define SLAVE_ADDRESS 0x40


// defining register pointers
#define CONFIG_REG_POINTER_ADRESS 0x00
#define SHUNT_REG_POINTER_ADRESS 0x01

/**
 * @brief sends an array of data via I2C to the specified slave
 * 
 * @param slave_addr address of slave component
 * @param array array of message to send
 */


void I2CSendString(uint32_t slave_addr, char array[])
{
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CSlaveACKOverride(I2C0_BASE, true);
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


/**
 * @brief Read from slave's register
 *
 * @param slave_addr Address of slave component
 * @param reg Register bit to be read
 * @return uint32_t data read from reg bit on slave
 */
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg)
{
    //specify that we are writing (a register address) to the
    //slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, false);

    //specify register to be read
    I2CMasterDataPut(I2C0_BASE, reg);

    //send control byte and register address byte to slave device
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);


    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //specify that we are going to read from slave device
    I2CMasterSlaveAddrSet(I2C0_BASE, slave_addr, true);

    //send control byte and read from the register we specified
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

    //wait for MCU to finish transaction
    while(I2CMasterBusy(I2C0_BASE));

    //return data pulled from the specified register
    return I2CMasterDataGet(I2C0_BASE);
}

//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
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

void UARTPrintOut(const uint8_t *pui8Buffer){
    // print chars until '\0' char is found
    while(*pui8Buffer != '\0'){
        UARTCharPut(UART0_BASE, *pui8Buffer);
        pui8Buffer++;
    }

}

//*****************************************************************************
//
// Configure the I2C0 master and slave and connect them using loopback mode.
//
//*****************************************************************************
int main(void)
{

    uint32_t pui32DataTx[NUM_I2C_DATA];
    uint32_t pui32DataRx[NUM_I2C_DATA];
    uint32_t ui32Index;

    //
    // Set the clocking to run directly from the external crystal/oscillator.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //

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
    while(I2CMasterBusy(I2C0_BASE));



//    initI2C();

    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);//setting up for write
    while(I2CMasterBusy(I2C0_BASE));

    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for I2C operation.
    //
    InitConsole();

    //
    // Display the example setup on the console.
    //
    UARTPrintOut("\r\nI2C-AUX MCU\r\n");
    


    //
    // Initalize the receive buffer.
    //
    for(ui32Index = 0; ui32Index < NUM_I2C_DATA; ui32Index++)
    {
        pui32DataRx[ui32Index] = 0;
    }

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while(I2CMasterBusy(I2C0_BASE));



    //
    // Indicate the direction of the data.
    //
    UARTPrintOut("\rWriting to config register to set register pointer to it to read from it\n");

//    pui32DataRx[5]= I2CReceive(SLAVE_ADDRESS ,CONFIG_REG_POINTER_ADRESS);

         I2CMasterDataPut(I2C0_BASE, CONFIG_REG_POINTER_ADRESS);//sending reg pointer address for configuration register


         I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);


         while(I2CMasterBusy(I2C0_BASE));

//char data[] = {0x00, 0x12, 0x99, '\0'};
//
//I2CSendString(SLAVE_ADDRESS,data);



   
    UARTPrintOut("\r\n\nNow trying to read the config which should be set by default to 0x399F\n");

    //
    // Modifiy the data direction to true, so that seeing the address will
    // indicate that the I2C Master is initiating a read from the slave.
    //
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, true);



    //
    // Do a dummy receive to make sure you don't get junk on the first receive.
    //
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

//    pui32DataRx[0] = I2CReceive(SLAVE_ADDRESS, CONFIG_REG_POINTER_ADRESS);



        //
        // Tell the master to read data.
        //
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

    for(ui32Index = 0; ui32Index < 4; ui32Index++)
    {
//
//        //
//        // Read the data from the master.
//        //
        while(I2CMasterBusy(I2C0_BASE));
        pui32DataRx[ui32Index] = I2CMasterDataGet(I2C0_BASE);


        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);




        //
        // Display the data that the slave has received.
        //
        UARTPrintOut("\rReceived: ");
        UARTCharPut(UART0_BASE,pui32DataRx[ui32Index]);
        UARTPrintOut("\r\n");
    }
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    //
    // Tell the user that the test is done.
    //
    UARTPrintOut("\r\nDone.\n\n");
while(1){}
    //
    // Return no errors
    //
    return(0);
}
