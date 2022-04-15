/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup group_common_boards
 * \defgroup user_board_group User board
 *
 * @{
 */

void system_board_init(void);

/** Name string macro */
#define BOARD_NAME                "Temperature Monitor M0+ V1"

/** @} */

#ifdef __cplusplus
}
#endif



///////////////////////// Pin Name Macros ////////////////////////////

#define TX_LED_PIN PIN_PA02
#define RX_LED_PIN PIN_PA03
#define DC_PG_PIN PIN_PA04			// Power good line off 5V regulator
#define CHG_STAT_PIN PIN_PA05		// Charging status pin from LiPo charger (tri-state pin)
#define NP_LED_CLK_PIN PIN_PA06		// Neopixel LED communication pins
#define NP_LED_DATA_PIN PIN_PA07
#define WIFI_RST_PIN PIN_PA08		// Controls WiFi module's reset line (active high)
#define LIGHT_INT_PIN PIN_PA09		// Configurable interrupt pin from the light sensor
#define BAT_MONITOR_PIN PIN_PA10	// Analog voltage pin to monitor LiPo battery level
#define PIR_SENSOR_PIN PIN_PA11		// Digital input pin from PIR motion sensor
#define WIFI_SPI_MOSI_PIN PIN_PA12	// Primary communication interface for the WiFi radio (SERCOM 2)
#define WIFI_SPI_MISO_PIN PIN_PA13
#define WIFI_SPI_CS_PIN PIN_PA14
#define WIFI_SPI_SCK_PIN PIN_PA15
#define CHG_CTRL_PIN PIN_PA16		// Charge control pin, active low, pulled low by external resistor
#define STAT_LED_PIN PIN_PA17		// MCU status LED
#define USB_HOST_EN_PIN PIN_PA18	// Detect if USB host is present (active high)
#define WIFI_STAT_R_PIN PIN_PA19	// WiFi status pin - repurposing RGB lines as general purpose status lines
#define WIFI_STAT_G_PIN PIN_PA20
#define WIFI_STAT_B_PIN PIN_PA21
#define MCU_TX_WIFI_RX_PIN PIN_PA22	// WiFi radio UART communication lines - used for radio firmware updates
#define MCU_RX_WIFI_TX_PIN PIN_PA23
#define USB_DM_PIN_PIN_PA24			// USB D-
#define USB_DP_PIN_PIN_PA25			// USB D+
#define WIFI_GPIO0_PIN PIN_PA27		// WiFi radio bootloader request pin used for radio firmware updates
#define WIFI_ACK_PIN PIN_PA28		// WiFi application handshake pin
#define FLASH_CS_PIN PIN_PB02		// Flash chip select pin (SERCOM 5)
#define FLASH_MISO_PIN PIN_PB03		// Flash SPI communication pin
#define I2C_SDA_PIN PIN_PB08		// I2C (SERCOM 4) - temperature/humidity, light sensor, ECC508, OLED screen
#define I2C_SCL_PIN PIN_PB09
#define WIFI_UART_RTS_PIN PIN_PB10	// WiFi UART handshake pins, NOT tied to the hardware peripheral
#define WIFI_UART_CTS_PIN PIN_PB11
#define FLASH_MOSI_PIN PIN_PB22		// Flash SPI communicatino pins (SERCOM 5)
#define FLASH_SCK_PIN PIN_PB23



// WiFi Radio Pin definitions - used for controlling radio GPIO
#define WIFI_GPIO_R_PIN 26 // Using R pin to indicate WiFi configuration setup
#define WIFI_GPIO_G_PIN 25
#define WIFI_GPIO_B_PIN 27


#endif // USER_BOARD_H
