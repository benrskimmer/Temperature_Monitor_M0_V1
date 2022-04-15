/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

#if defined(__GNUC__)
void board_init(void) WEAK __attribute__((alias("system_board_init")));
#elif defined(__ICCARM__)
void board_init(void);
#  pragma weak board_init=system_board_init
#endif

void system_board_init(void)
{	
	
	// LEDs - apa102 pins are handled by their own init function
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(STAT_LED_PIN, &config_port_pin);
 	port_pin_set_output_level(STAT_LED_PIN, 0); // Set LED to OFF
	 
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TX_LED_PIN, &config_port_pin);
	port_pin_set_output_level(TX_LED_PIN, 0); // Set LED to OFF
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(RX_LED_PIN, &config_port_pin);
	port_pin_set_output_level(RX_LED_PIN, 0); // Set LED to OFF
	
	
	
	// Sensors
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(PIR_SENSOR_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(LIGHT_INT_PIN, &config_port_pin);
	
	
	
	// WiFi Radio pins
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT; // configure as output when transmitting, leave as input to allow external reflashing
	port_pin_set_config(MCU_TX_WIFI_RX_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(MCU_RX_WIFI_TX_PIN, &config_port_pin);
	
// 	port_get_config_defaults(&config_port_pin); // leaving these uninitialized for now
// 	config_port_pin.direction = PORT_PIN_DIR_INPUT;
// 	port_pin_set_config(WIFI_UART_RTS_PIN, &config_port_pin);
// 	
// 	port_get_config_defaults(&config_port_pin);
// 	config_port_pin.direction = PORT_PIN_DIR_INPUT;
// 	port_pin_set_config(WIFI_UART_CTS_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_output_level(WIFI_GPIO0_PIN, 0); // set bootload request pin to low (active high)
	port_pin_set_config(WIFI_GPIO0_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_output_level(WIFI_RST_PIN, 0); // Set RST to low (active high)
	port_pin_set_config(WIFI_RST_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(WIFI_ACK_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(WIFI_STAT_R_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(WIFI_STAT_G_PIN, &config_port_pin);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(WIFI_STAT_B_PIN, &config_port_pin);
	
	
	// USB pins	
	struct system_pinmux_config DP;
	system_pinmux_get_config_defaults(&DP);
	DP.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	DP.mux_position = MUX_PA25G_USB_DP;
	system_pinmux_pin_set_config(PIN_PA25G_USB_DP, &DP);
		
	struct system_pinmux_config DM;
	system_pinmux_get_config_defaults(&DM);
	DM.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	DM.mux_position = MUX_PA24G_USB_DM;
	system_pinmux_pin_set_config(PIN_PA24G_USB_DM, &DM);
	
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(USB_HOST_EN_PIN, &config_port_pin);
	
	
	
	
	
	
	
	
	
	
	
	
	// Clock testing
/*
//GCLK3 testing - enables outputs
	struct system_pinmux_config GCLK3OUT;
	system_pinmux_get_config_defaults(&GCLK3OUT);
	GCLK3OUT.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	GCLK3OUT.mux_position = MUX_PA17H_GCLK_IO3;
	system_pinmux_pin_set_config(LED_PIN, &GCLK3OUT);
	
//GCLK0 testing - enables outputs
	struct system_pinmux_config GCLK0OUT;
	system_pinmux_get_config_defaults(&GCLK0OUT);
	GCLK0OUT.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	GCLK0OUT.mux_position = MUX_PB22H_GCLK_IO0;
	system_pinmux_pin_set_config(PIN_PB22, &GCLK0OUT);
*/
}