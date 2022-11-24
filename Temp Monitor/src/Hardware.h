/*
 * Board.h
 *
 * Created: 4/14/2022 3:38:40 AM
 *  Author: Ben
 */ 


#ifndef HARDWARE_H_
#define HARDWARE_H_

// NOTE: include all peripheral drivers here so anything pulling in sensors has access to the drivers
#include "light_apa102.h"
//#include "ss_oled.h"
#include "SSD1306Ascii.h"
#include "Si7021.h"
#include "VEML6030.h"


void hardwareInit(void);



#endif /* HARDWARE_H_ */