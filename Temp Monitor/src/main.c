/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <spi.h>
#include <port.h>
#include <sercom.h>
#include <pinmux.h>
#include <clock.h>
#include <stdio.h>
#include "millis.h"
#include "i2c_master.h"
#include "ss_oled.h"
#include "light_apa102.h"
#include "conf_clocks.h"
#include "Si7021.h"
#include "VEML6030.h"
#include "Remote.h"
//#include "ECCX08/ECCX08.h"
//#include "ECCX08/utility/ECCX08DefaultTLSConfig.h"

#define BUF_LENGTH 20

#define SLAVE_SELECT_PIN PIN_PA14

#define LED_PIN PIN_PA17
#define PIR_PIN PIN_PA11




struct i2c_master_module i2c_master_instance;
	
static uint8_t ucBackBuffer[1024];

static uint8_t buffer[BUF_LENGTH] = {
	0xBB, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13
};


static bool my_flag_autorize_cdc_transfert = false;
bool my_callback_cdc_enable(void)
{
	stdio_usb_enable();
	my_flag_autorize_cdc_transfert = true;
	return true;
}
void my_callback_cdc_disable(void)
{
	stdio_usb_disable();
	my_flag_autorize_cdc_transfert = false;
}


void vbus_event(bool b_vbus_high) {
	if (b_vbus_high) {
		// Connect USB device
		udc_attach();
		}else{
		// Disconnect USB device
		udc_detach();
	}
}

// Possible values: .125, .25, 1, 2
// Both .125 and .25 should be used in most cases except darker rooms.
// A gain of 2 should only be used if the sensor will be covered by a dark
// glass.
float gain = .125;

// Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
// Higher times give higher resolutions and should be used in darker light.
int integ_time = 100;
long luxVal = 0;

// cdc_enable() {
// // Open UART and enable UART transition interrupts
// 
// return true;
// }
// cdc_disable() {
// // Disable UART interrupts and close UART
// 
// } 
/*
void listNetworks() {
	// scan for nearby networks:
	printf("** Scan Networks **\n");
	int numSsid = WiFi.scanNetworks();
	if (numSsid == -1) {
		printf("Couldn't get a wifi connection\n");
		while (true);
	}

	// print the list of networks seen:
	printf("number of available networks:");
	printf(numSsid);
	printf("\n");

	// print the network number and name for each network found:
	for (int thisNet = 0; thisNet < numSsid; thisNet++) {
		printf(thisNet);
		printf(") ");
		printf(WiFi.SSID(thisNet));
		printf("\tSignal: ");
		printf(WiFi.RSSI(thisNet));
		printf(" dBm");
		printf("\tEncryption: ");
		printEncryptionType(WiFi.encryptionType(thisNet));
	}
}

void printEncryptionType(int thisType) {
	// read the encryption type and print out the name:
	switch (thisType) {
		case ENC_TYPE_WEP:
		printf("WEP");
		break;
		case ENC_TYPE_TKIP:
		printf("WPA");
		break;
		case ENC_TYPE_CCMP:
		printf("WPA2");
		break;
		case ENC_TYPE_NONE:
		printf("None");
		break;
		case ENC_TYPE_AUTO:
		printf("Auto");
		break;
		case ENC_TYPE_UNKNOWN:
		default:
		printf("Unknown");
		break;
	}
}
*/




int EndsWith(const char *str, const char *suffix)
{
	if (!str || !suffix)
	return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix >  lenstr)
	return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}


void configure_i2c_master(void)
{
	volatile enum status_code result = STATUS_BUSY;
	
	/* Initialize config structure and software module. */
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	/* Change buffer timeout to something longer. */
	config_i2c_master.buffer_timeout = 10000;
	//	config_i2c_master.sda_scl_rise_time_ns = 215;
	config_i2c_master.pinmux_pad0 = PINMUX_PB08D_SERCOM4_PAD0;
	config_i2c_master.pinmux_pad1 = PINMUX_PB09D_SERCOM4_PAD1;
	config_i2c_master.generator_source = GCLK_GENERATOR_3;
	
	/* Initialize and enable device with config. */
	result = i2c_master_init(&i2c_master_instance, SERCOM4, &config_i2c_master);
	
	i2c_master_enable(&i2c_master_instance);
}



int main (void)
{
	system_init();
	
	millis_init();
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_PIN, &config_port_pin);
// 	
// 	port_pin_set_output_level(LED_PIN, 1); // Set LED to ON

	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	port_pin_set_config(PIR_PIN, &config_port_pin);

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

	/* Insert application code here, after the board has been initialized. */
	
	
	// Authorize interrupts
	irq_initialize_vectors();
	cpu_irq_enable();
	// Initialize the sleep manager service
//	sleepmgr_init();

	
	udc_start();
	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		vbus_event (true);
	}
	
	stdio_usb_init();
//	stdio_usb_enable();

	
	
	uint8_t ch_from_USB = 0;
	uint8_t ch_from_SPI = 0;
	
// 	check for the WiFi module:
// 		if (WiFi.status() == WL_NO_MODULE) {
// 			printf("Communication with WiFi module failed!\n");
// 			// don't continue
// 			while (true);
// 		}
// 	
// 		char fv[20] = WiFi.firmwareVersion();
// 		printf("%s", fv);
// 		if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
// 			printf("Please upgrade the firmware\n");
// 		}
// 	
// 		// print your MAC address:
// 		uint8_t mac[6];
// 		WiFi.macAddress(mac);
// 		printf("MAC: ");
// 		printMacAddress(mac);
// 		
// 		printf("Scanning available networks...");
// 		listNetworks();

	configure_i2c_master();

	SSOLED screen;
	

	char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D",(char *)"SH1106 @ 0x3C",(char *)"SH1106 @ 0x3D"};
	//	i2c_master_write_packet_wait(&i2c_master_instance, &packet);
	int rc = oledInit(&screen, &i2c_master_instance, OLED_128x32, 0xFF, 0, 0);
	if (rc != OLED_NOT_FOUND)
	{
		oledFill(&screen, 0, 1);
		oledWriteString(&screen, 0,0,0,msgs[rc], FONT_NORMAL, 0, 1);
		oledSetBackBuffer(&screen, ucBackBuffer);
		delay(1000);
	}
//	oledSetContrast(&screen, 0);
//	oledPower(&screen, 0);
	msgs[0] = (char *)"Hello!!";
	oledFill(&screen, 0, 1);
	oledSetTextWrap(&screen, 1);
	oledWriteString(&screen, 0,35,1,msgs[rc], FONT_NORMAL, 0, 1);
	msgs[0] = (char *)"IP: 192.168.4.1";
	oledWriteString(&screen, 0,0,3,msgs[rc], FONT_NORMAL, 0, 1);

	cRGB_t LED = {0, 100, 0};
	apa102_init(PIN_PA06, PIN_PA07);
	apa102_setleds(&LED, 1);

	//ECCX08_init(&i2c_master_instance);
	//	printf("%d", system_cpu_clock_get_hz());

	initRemote();
	syncRemote();
	
	
	Si7021_begin(&i2c_master_instance);
	VEML6030_begin(&i2c_master_instance);
	
	VEML6030_setGain(gain);
	VEML6030_setIntegTime(integ_time);
	
// 	Si7021_setHeatLevel(0x0F);
// 	Si7021_heater(TRUE);
	
	uint32_t temp_millis = 0;
	
	while(1){
		
// 		if(STATUS_OK == spi_read_buffer_wait(&spi_master_instance, &ch_from_SPI, 1, 0xFF))
// 		{
// 			printf("%c",ch_from_SPI);
// 		}
		
//		spi_select_slave(&spi_master_instance, &slave, false);
//		udi_cdc_putc(0x42);
// 		printf("Fuck Yeah!\n");
// 
// 		scanf("%c",&ch_from_USB); // get one input character
// 		
// 		printf("\nyou sent %c\n", ch_from_USB);
		
// 		if (ch_from_USB)
// 		{
// 			spi_write_buffer_wait(&spi_master_instance, &ch_from_USB, 1);
// 		}
		oledPower(&screen, port_pin_get_input_level(PIR_PIN));
		port_pin_set_output_level(LED_PIN, port_pin_get_input_level(PIR_PIN));

		if( millis() >= (temp_millis + 2000)) {
//			port_pin_toggle_output_level(LED_PIN);
			temp_millis = millis();
			
			printf("Temperature: %.2fC\nHumidity: %.2f\n", Si7021_readTemperature(), Si7021_readHumidity());
			printf("Light: %lu Lux\n\n", VEML6030_readLight());
		
			
// 			msgs[0] = (char *)"Hello!!";
// 			oledFill(&screen, 0, 1);
// 			oledWriteString(&screen, 0,35,1,msgs[rc], FONT_NORMAL, 0, 1);
		}
		uint32_t brightness = (VEML6030_readLight()>>2);
		if(brightness < 40) brightness = 40;
		else if(brightness > 150) brightness = 150;
		cRGB_t LED = {0, (uint8_t)brightness, 0};
		apa102_setleds(&LED, 1);
		delay(50);
	}
}
