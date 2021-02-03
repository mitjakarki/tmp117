/*
  tmp117.cpp - library for Texas Instruments tmp117 digital temperature sensor
  Copyright (c) 2020 Mitja Kärki
  University of Oulu
*/

/*
This library is for controlling Texas Instruments tmp117 digital temperature sensor.
The library was written as a part of my bachelor's thesis.

Constructed using the "Arduino style guide for writing libraries".
https://www.arduino.cc/en/Reference/APIStyleGuide
*/

#include "tmp117.h"

// Empty constructor
tmp117::tmp117() {}

// Begin function is used to initialise a library instance in arduino sketches.
bool tmp117::begin(uint8_t deviceAddress)
{
    /* tmp117 address on the i2c bus can be set to 4 different values by
    connecting the tmp117 ADD0 pin to other pins.
    See tmp117 datasheet table 2 for details.*/
    if ((deviceAddress < 72) || (deviceAddress > 75))
    {
        return false;
    }
    /* See datasheet pin functions table for pin ADD0 connection defined addresses. 
    Set to 0x48 (=72) for ground connection (default). */
    tmp117Address = deviceAddress;

    // Initialise library
    Wire.begin(); // Initialise wire library for i2c communication
    delay(2);     // 2 ms delay to allow the sensor to boot up
    
    // defaultConfig enables the alert functionality
    return i2cWriteData(configRegister, defaultConfig);
}

// Read temperature register
double tmp117::readTemperature(void)
{
    double temp;    // Double precision float for final return value
	temp = i2cReadData(temperatureRegister) * resolution;
    return temp;
}

// Set alert low and high limits 
bool tmp117::setAlertLimits(double lowLimit, double highLimit)
{   
    // Divide limit values by resolution and convert to integers
    uint16_t lowLimitInt = lowLimit/resolution;
    uint16_t highLimitInt = highLimit/resolution;

    // Write alert temperature limits to register and check if it was succesful 
    return (i2cWriteData(lowLimitRegister, lowLimitInt) &&
       (i2cWriteData(highLimitRegister, highLimitInt)));
}

// Read configuration register contents to see if alert flags are set
uint8_t tmp117::readAlert(void)
{
	// Store contents of configuration register here	
    uint16_t configReg;
    uint8_t alertState;
    // Configuration register bitmasks
    uint16_t alertBit       = 0b0000000000010000;
    uint16_t highAlertBit   = 0b1000000000000000;
    uint16_t lowAlertBit    = 0b0100000000000000;

	configReg = i2cReadData(configRegister);
    // Make sure that alert functionality is enabled
    // Alert mode is set if alert bit is set to 0 in config reg
    if (alertBit & ~configReg)
    {
        // Check if alert flags are set
        if (configReg & highAlertBit)
        {
            alertState = highAlert;
        }
        else if (configReg & lowAlertBit)
        {
            alertState = lowAlert;
        }
        else {
            alertState = noAlert;
        }
    }
    else
    {
        alertState = noAlert;
    }
    // Clear status flags by reading the tmp117 config register
    i2cReadData(configRegister);

    return alertState;
}

// Read given register contents
uint16_t tmp117::i2cReadData(uint8_t registerAddress)
{   
    // The arduino wire library reads one byte at a time from i2c line
    // so we need two one-byte variables to save the whole 16-bit temp value
    uint8_t msByte; // most significant byte of 16-bit temp reading
    uint8_t lsByte; // least significant byte
    
    // Begin transmission with tmp117
    // Master needs to address the slave for slave to transmit data.
    // Each i2c device has its own address on the bus
    // Master also needs to send START condition
    Wire.beginTransmission(tmp117Address);
    // Slave needs configuration from master.
    // Master needs to access slave's internal register map by writing the 
    Wire.write(registerAddress);
    // Master needs to send STOP condition to terminate the transfer.
    // In arduino wire lib the endTransmission transmits queued write()-bytes from master to slave
    Wire.endTransmission();
    // Some delay while the sensor converts temperature to digital format and handles averaging
    delay(conversionDelay);
    // Request 2 bytes of data (=contents of a register) from tmp117
    Wire.requestFrom(tmp117Address, byteAmount);
    
    if(Wire.available() == byteAmount);
    {
        msByte = Wire.read();
        lsByte = Wire.read();
        // Return data combined to a single 16-bit int
        return ((msByte << 8) | lsByte);
    }
}

// Writes a 16-bit packet of data to a register on the tmp117
bool tmp117::i2cWriteData(uint8_t registerAddress, uint16_t data)
{   
    // Counter for checking that the right amount of bytes were written
    uint8_t byteCounter = 0;
    // First split the 16-bit data into two 8-bit variables
    uint8_t highByte = data >> 8;
    uint8_t lowByte = data & 0xff;
    
    Wire.beginTransmission(tmp117Address);
    Wire.write(registerAddress);
    // Write the actual data and check that right amount of bytes were written
    byteCounter = byteCounter + Wire.write(highByte);
    byteCounter = byteCounter + Wire.write(lowByte);
    Wire.endTransmission();
    delay(2);
    // Successful write
    if (byteCounter == byteAmount)
    {
        return true;
    }
    // Unsuccessful write
    else {
        return false;
    }
}

