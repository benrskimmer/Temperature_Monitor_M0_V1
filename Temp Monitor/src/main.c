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
#include <stdio.h>
#include "millis.h"
#include "ss_oled.h"
#include "Hardware.h"
#include "Remote.h"
#include "Sensors.h"
#include "Application.h"
#include "user_board.h"
//#include "ECCX08/ECCX08.h"
//#include "ECCX08/utility/ECCX08DefaultTLSConfig.h"

#define BUF_LENGTH 20






extern struct i2c_master_module i2c_master_instance;
	


static uint8_t buffer[BUF_LENGTH] = {
	0xBB, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13
};




extern SSOLED screen;

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



int main (void)
{
	
	hardwareInit();
	/* Insert application code here, after the board has been initialized. */

	initApp();

	startApp();



	

	//ECCX08_init(&i2c_master_instance);
	//	printf("%d", system_cpu_clock_get_hz());

	
	
	

	

	
	uint32_t temp_millis = 0;
	
	while(1){
		runApp();
		runRemote();
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
		

		if( millis() >= (temp_millis + 2000)) {
//			port_pin_toggle_output_level(LED_PIN);
			temp_millis = millis();
			//oledPower(&screen, port_pin_get_input_level(PIR_SENSOR_PIN));
		
			//sensorsUpdate();
		
// 			printf("WiFi Radio R,G,B are %d,%d,%d,\n", 
// 				port_pin_get_input_level(WIFI_GPIO_R_PIN),
// 				port_pin_get_input_level(WIFI_GPIO_G_PIN),
// 				port_pin_get_input_level(WIFI_GPIO_B_PIN));
		}

	}
}
