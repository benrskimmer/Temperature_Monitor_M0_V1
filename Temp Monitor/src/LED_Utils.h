/*
 * LED_Utils.h
 *
 * Created: 4/22/2022 3:27:27 AM
 *  Author: Ben
 */ 


#ifndef LED_UTILS_H_
#define LED_UTILS_H_
#include <stdint-gcc.h>

///////////////////////// PUBLIC DEFINES /////////////////////////
#define LED_FULL_BRIGHTNESS 100


///////////////////// PUBLIC TYPE DEFINITIONS /////////////////////
typedef enum colors{
	RED = 0,
	GREEN,
	BLUE,
	WHITE,
	YELLOW,
	ORRANGE,
	PURPLE,
	CYAN,
	MAGENTA,	
} colors_t;

// typedef struct rgb{
// 	uint8_t r;
// 	uint8_t g;
// 	uint8_t b;
// } rgb_t;

//////////////////////// PUBLIC FUNCTIONS ////////////////////////
void setUserLEDColor(colors_t color, uint8_t brightness);
void setBrightness(uint8_t brightness); // updates the brightness of the existing stored color



#endif /* LED_UTILS_H_ */