/*
 * LED_Utils.c
 *
 * Created: 4/22/2022 3:27:55 AM
 *  Author: Ben
 */ 

#include "LED_Utils.h"
#include "light_apa102.h"

//////////////////////// PRIVATE VARIABLES ////////////////////////
rgb_t led_raw = {0,0,0}; // Keeps track of last color setting
	
//////////////////////// PRIVATE FUNCTIONS ////////////////////////
void calculateOutputBrightness(rgb_t input, rgb_t* output, uint8_t brightness)
{
	// calculate the brightness based on the relative inputs above
	output->r = (uint8_t)((float)brightness/100*input.r);
	output->g = (uint8_t)((float)brightness/100*input.g);
	output->b = (uint8_t)((float)brightness/100*input.b);
}

//////////////////////// PUBLIC FUNCTIONS ////////////////////////

/************************************************************************
Description: Sets LED to a preset color with a brightness as a percent between 0 and 100
where 0 is the dimmest setting with the LED still on and 100 is full brightness.
************************************************************************/

void setUserLEDColor(colors_t color, uint8_t brightness)
{
	if(brightness > 100) brightness = 100;
	
	led_raw.r = led_raw.g = led_raw.b = 0;
	switch(color)
	{
		case RED:
			led_raw.r = 255;
		break;
		case GREEN:
			led_raw.g = 255;
		break;
		case BLUE:
			led_raw.b = 255;
		break;
		case WHITE:
			led_raw.r = 210;
			led_raw.g = 255;
			led_raw.b = 205;
		break;
		case YELLOW:
			
		break;
		case ORRANGE:
			
		break;
		case PURPLE:
			
		break;
		case CYAN:
			
		break;
		case MAGENTA:
			
		break;
		default:
			return;
	}
	
	rgb_t led_output = {0,0,0};
		
	calculateOutputBrightness(led_raw, &led_output, brightness);
	
	apa102_setleds(&led_output, 1);
}

void setBrightness(uint8_t brightness)
{
	if(brightness > 100) brightness = 100;
	
	rgb_t led_output = {0,0,0};
		
	calculateOutputBrightness(led_raw, &led_output, brightness);
	
	apa102_setleds(&led_output, 1);
}