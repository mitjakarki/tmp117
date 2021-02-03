/*
	Example code for tmp117 library

	Showcase tmp117 library by creating a window detector for monitoring room temperature.

	The circuit:
    - Texas Instruments TMP117 attached to i2c bus (pins depend on arduino board version)
        - use TMP117 datasheet as reference: https://www.ti.com/lit/ds/symlink/tmp117.pdf
            - you'll need 3 * 5 kOhm resistors
        - tmp117 VDD to arduino 3.3 volt output
        - tmp117 GND to arduino GND
        - tmp117 ADD0 to GND (to select address on i2c bus)
    - 10 nF capacitor between VDD and GND to smooth out voltage source ripple
	Created in 2020
	By Mitja Kärki

	https://github.com/mitjakarki/tmp117

*/

#include <tmp117.h>

// create an instance of the tmp117 class
tmp117 thermometer;

const uint8_t tmp117Address = 72;   // tmp117 ADD0 pin connected to GND
const double  alertLowLim   = 23.0; // limits in celsius
const double  alertHiLim    = 25.0;

void setup() {
    // setup runs once
    Serial.begin(9600);
    delay(2000);
    Serial.println("Starting temperature measurement");
    // run library init function and
    // check that the sensor address is correct
    if (!thermometer.begin(tmp117Address))
    {
        Serial.println("Invalid tmp117 address!");
        while (1) {
        delay(2000);
        }
    }
    // set alert low and high limits
    bool alert = thermometer.setAlertLimits(alertLowLim, alertHiLim);
    Serial.println("Setting alert limits was successful?");
    Serial.println(alert);
}

void loop() {
    // loop runs repeatedly
    // read and print temperature
    double temp = thermometer.readTemperature();
    Serial.println("Measured temperature is:");
    Serial.println(temp);
  
    // read and print alert status
    uint8_t alert = thermometer.readAlert();
    if (alert == thermometer.noAlert) {
        Serial.println("Temperature within set window");
    }
    else if (alert == thermometer.highAlert) {
        Serial.println("ALERT! Temperature higher than allowed");
    }
    else if (alert == thermometer.lowAlert) {
        Serial.println("ALERT! Temperature lower than allowed");
    }
    else {
        Serial.println("Reading alert status went wrong");
        Serial.println(alert);
    }
    delay(3000);
}
