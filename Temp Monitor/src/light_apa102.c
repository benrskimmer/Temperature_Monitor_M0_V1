/*
* light weight APA102 lib - v0.1
*
* Controls APA102 RGB-LEDs
* Author: Tim (cpldcpu@gmail.com)
*
* Nov 30th, 2014 Initial Version
*
* License: GNU GPL v2 (see License.txt)
*/

#include "light_apa102.h"
#include <asf.h>
#include <port.h>

#define nop() asm volatile(" nop \n\t")

uint8_t apa102_clk_pin, apa102_data_pin = 0;

/*
  Software based SPI implementation. f_clk is approximately f_cpu/16.
  This is not optimized for maximum speed, but to output proper waveforms in case of
  bad cabling. If you want more speed, you should adapt this code to use
  hardware SPI.
*/

void apa102_init(uint8_t apa102_clk, uint8_t apa102_data) {
	
	apa102_clk_pin = apa102_clk;
	apa102_data_pin = apa102_data;
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(apa102_clk_pin, &config_port_pin);
	port_pin_set_config(apa102_data_pin, &config_port_pin);
	
	port_pin_set_output_level(apa102_clk_pin, 0);
	port_pin_set_output_level(apa102_data_pin, 1);
	
}

// Assumed state before call: SCK- Low, Dat- High
void apa102_write(uint8_t c) {
  uint8_t i;
  for (i=0; i<8 ;i++)
  {
    if (!(c&0x80)) {
      port_pin_set_output_level(apa102_data_pin, 0); // set data low
    } else {
      port_pin_set_output_level(apa102_data_pin, 1); // set data high
    }     
//   nop();
//   nop();

  port_pin_set_output_level(apa102_clk_pin, 1); // SCK hi , data sampled here

  c<<=1;
  
  nop();  // Stretch clock
  nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();
//   nop();

  
  port_pin_set_output_level(apa102_clk_pin, 0); // clk low
  }
// State after call: SCK Low, Dat high
port_pin_set_output_level(apa102_data_pin, 1); // clk low
}

void inline apa102_setleds(rgb_t *ledarray, uint16_t leds)
{
  apa102_setleds_brightness(ledarray,leds,31);
}
 
void inline apa102_setleds_brightness(rgb_t *ledarray, uint16_t leds,uint8_t brightness)
{
  uint16_t i;
  uint8_t *rawarray=(uint8_t*)ledarray;
 // apa102_init();
  
  apa102_write(0x00);  // Start Frame 
  apa102_write(0x00);
  apa102_write(0x00);
  apa102_write(0x00);
 
  for (i=0; i<(leds+leds+leds); i+=3)
  {
    apa102_write(0xe0+brightness);  // Maximum global brightness
    apa102_write(rawarray[i+2]);
    apa102_write(rawarray[i+1]);
    apa102_write(rawarray[i+0]);
  }

  // Reset frame - Only needed for SK9822, has no effect on APA102
//   apa102_write(0x00);  
//   apa102_write(0x00);
//   apa102_write(0x00);
//   apa102_write(0x00);
  
  // End frame: 8+8*(leds >> 4) clock cycles    
  for (i=0; i<leds; i+=16)
  {
    apa102_write(0x00);  // 8 more clock cycles
  }
}
