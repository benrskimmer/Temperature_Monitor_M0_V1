/*
 * Board.c
 *
 * Created: 4/14/2022 3:39:12 AM
 *  Author: Ben
 */ 

#include <asf.h>
#include <clock.h>
#include <spi.h>
#include <port.h>
#include <sercom.h>
#include <pinmux.h>
#include "Hardware.h"
#include "i2c_master.h"
#include "conf_clocks.h"
#include "millis.h"


///////////////////////////////////// private variables ///////////////////////////////////


/////////////////////////////////// public variables ///////////////////////////////////
struct i2c_master_module i2c_master_instance;
struct rtc_module rtc_instance;
//SSOLED screen;

static uint8_t ucBackBuffer[1024]; // temporary


/////////////////////////////////// private functions ///////////////////////////////////

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
	config_i2c_master.baud_rate = I2C_MASTER_BAUD_RATE_400KHZ;
	
	/* Initialize and enable device with config. */
	result = i2c_master_init(&i2c_master_instance, SERCOM4, &config_i2c_master);
	
	i2c_master_enable(&i2c_master_instance);
}

void configure_rtc_calendar(void)
{
	/* Initialize RTC in calendar mode. */
	struct rtc_calendar_config config_rtc_calendar;
	
	rtc_calendar_get_config_defaults(&config_rtc_calendar);
	config_rtc_calendar.clock_24h = true;
	
	rtc_calendar_init(&rtc_instance, RTC, &config_rtc_calendar);
	rtc_instance.hw->MODE2.DBGCTRL.bit.DBGRUN |= 0b1; // allow RTC to run with debugger halted
	rtc_instance.hw->MODE2.FREQCORR.bit.SIGN = 0b0;
	rtc_instance.hw->MODE2.FREQCORR.bit.VALUE = 0b1111111; // frequency correction - max decrease
	rtc_calendar_enable(&rtc_instance);
}


static bool my_flag_autorize_cdc_transfert = false;
bool usb_callback_cdc_enable(void)
{
	stdio_usb_enable();
	my_flag_autorize_cdc_transfert = true;
	return true;
}
void usb_callback_cdc_disable(void)
{
	stdio_usb_disable();
	my_flag_autorize_cdc_transfert = false;
}
void usb_callback_rx_notify(uint8_t port)
{
	port_pin_set_output_level(RX_LED_PIN, 1);
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



/////////////////////////////////// public functions ///////////////////////////////////

void hardwareInit(void){
	
	system_init(); // this calls system_board_init and sets up GPIOs
	
	millis_init();
	
	// Authorize interrupts
	irq_initialize_vectors();
	cpu_irq_enable();
	// Initialize the sleep manager service
	//	sleepmgr_init();

	
	// Setup USB port
	udc_start();
	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		vbus_event (true);
	}
	
	stdio_usb_init();
	//	stdio_usb_enable();
	
	// configure i2c bus
	configure_i2c_master();
	
	// configure RTC calendar
	configure_rtc_calendar();
	
	// configure peripherals
	apa102_init(NP_LED_CLK_PIN, NP_LED_DATA_PIN);
	Si7021_begin(&i2c_master_instance);
 	VEML6030_begin(&i2c_master_instance);
// 	int rc = 0;//oledInit(&screen, &i2c_master_instance, OLED_128x32, 0xFF, 0, 0);
// 	
// 	char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D",(char *)"SH1106 @ 0x3C",(char *)"SH1106 @ 0x3D"};
// 	if (rc != OLED_NOT_FOUND)
// 	{
// 		oledFill(&screen, 0, 1);
// 		oledWriteString(&screen, 0,0,0,msgs[rc], FONT_NORMAL, 0, 1);
// 		oledSetBackBuffer(&screen, ucBackBuffer);
// 		delay(1000);
// 	}
// 	//	oledSetContrast(&screen, 0);
// 	//	oledPower(&screen, 0);
// 	msgs[0] = (char *)"Hello!!";
// 	oledFill(&screen, 0, 1);
// 	oledSetTextWrap(&screen, 1);
// 	oledWriteString(&screen, 0,35,1,msgs[rc], FONT_NORMAL, 0, 1);
// 	msgs[0] = (char *)"IP: 192.168.4.1";
// 	oledWriteString(&screen, 0,0,3,msgs[rc], FONT_NORMAL, 0, 1);
	
	
	SSD1306_begin(0x3C);
	SSD1306_setFont(lcd5x7);
	
// 	for(int i = 0; i < 20; i++)
// 	{
// 		SSD1306_write_char('A');
// 		SSD1306_write_char('B');
// 		SSD1306_write_char('C');
// 		SSD1306_write_char('D');
// 		SSD1306_write_char('E');
// 		SSD1306_write_char('F');
// 		SSD1306_write_char('\n');
// 	}
// 	int scroll = 0;
// 	for (int i = 0; i < 32; ++i) {
// 		delay(100);
// 		SSD1306_WriteCmd(SSD1306_SETSTARTLINE | scroll % 64);
// 		++scroll;
// 	}
// 	SSD1306_WriteCmd(SSD1306_SETSTARTLINE | scroll % 64);
// 	SSD1306_oledPower(0);
}

