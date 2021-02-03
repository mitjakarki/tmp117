/*
  tmp117.h - Library for TI tmp117 temperature sensor
  Copyright (c) 2020 Mitja Kärki
  University of Oulu
*/

// Ensure this library description is only included once
#ifndef tmp117_h
#define tmp117_h

#include "Arduino.h"    // For using the delay()-function in the library
#include "Wire.h"       // For i2c communication

// Declare the class
class tmp117
{
    // User-accessible public interface
    public:
        // Functions
        tmp117(); // Class constructor. Left empty in arduino libraries.
        double readTemperature(void);
        bool setAlertLimits(double low, double high);
        uint8_t readAlert(void);            // Checks if temperature is within the set alert limit
        bool begin(uint8_t deviceAddress);  // Library init function
		// Variables
		// readAlert return values
		const uint8_t noAlert   = 0b00; // temp measurement between alert limits
		const uint8_t highAlert = 0b10; // temp measurement above high limit
		const uint8_t lowAlert  = 0b01; // temp measurement below low limit

    // Library-accessible private interface
    private:
        // Functions
        bool i2cWriteData(uint8_t registerAddress, uint16_t data);
        uint16_t i2cReadData(uint8_t registerAddress);
        // Variables
        const uint8_t byteAmount = 2; // All data is 2 bytes of size
        uint8_t tmp117Address = 72;   // Set 72 (ADD0 pin in GND) as default i2c address
        float resolution = 0.0078125; // LSB resolution of tmp117
        const uint16_t defaultConfig = 0b0000001000100000; // Config register default (reset) values
		// tmp117 internal register addresses
		const uint8_t temperatureRegister	= 0x00;
		const uint8_t configRegister		= 0x01;
		const uint8_t highLimitRegister    	= 0x02;
		const uint8_t lowLimitRegister     	= 0x03;
		const uint16_t conversionDelay 		= 300;  // default conversion and averaging time in ms
};
#endif // tmp117

