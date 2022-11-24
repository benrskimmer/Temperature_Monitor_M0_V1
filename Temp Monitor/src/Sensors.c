/*
 * Sensors.c
 *
 * Created: 4/12/2022 2:00:31 AM
 *  Author: Ben
 */ 

#include "Sensors.h"
#include <stdio.h>
#include "Si7021.h"
#include "VEML6030.h"


//////////////////////// Sensor Configuration Defines /////////////////////////

// Possible values: .125, .25, 1, 2
// Both .125 and .25 should be used in most cases except darker rooms.
// A gain of 2 should only be used if the sensor will be covered by a dark
// glass.
#define GAIN 0.125

// Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
// Higher times give higher resolutions and should be used in darker light.
#define INTEG_TIME 100



////////////////////////////// Private Variables //////////////////////////////
//long luxVal = 0;


////////////////////////////// Pbulic Variables ///////////////////////////////



BOOL configureSensors(void)
{
		VEML6030_setGain(GAIN);
		VEML6030_setIntegTime(INTEG_TIME);
		// 	Si7021_setHeatLevel(0x0F);
		Si7021_heater(FALSE);
}


void sensorsUpdate(void)
{
	//printf("Temperature: %.2fC\nHumidity: %.2f\n", Si7021_readTemperature(), Si7021_readHumidity());
	//printf("Light: %lu Lux\n\n", VEML6030_readLight());
}