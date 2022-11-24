/*
spi_drv.h - Library for Arduino Wifi shield.
Copyright (c) 2018 Arduino SA. All rights reserved.
Copyright (c) 2011-2014 Arduino. All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef WiFi_SPI_Drv_h
#define WiFi_SPI_Drv_h

#include "compiler.h"
#include "preprocessor.h"
#include <inttypes.h>
#include <stdio.h>
#include "wifi_spi.h"

#define SPI_START_CMD_DELAY 10

#define NO_LAST_PARAM 0
#define LAST_PARAM 1

#define DUMMY_DATA 0xFF

extern bool wifi_initialized;

#define WAIT_FOR_SLAVE_SELECT()	\
if (!wifi_initialized) {			\
	wifi_spi_begin();			\
}								\
wifi_spi_waitForSlaveReady();	\
wifi_spi_spiSlaveSelect();


//private:
// bool waitSlaveReady();
// void waitForSlaveSign(void);
// void wifi_spi_getParam(uint8_t* param);
//public:


void wifi_spi_begin(void);
void wifi_spi_end(void);
void wifi_spi_spiDriverInit(void);
void wifi_spi_spiSlaveSelect(void);
void wifi_spi_spiSlaveDeselect(void);
char wifi_spi_spiTransfer(volatile char data);
void wifi_spi_waitForSlaveReady(void);
// int waitSpiChar(char waitChar, char* readChar);
int wifi_spi_waitSpiChar(unsigned char waitChar);
int wifi_spi_readAndCheckChar(char checkChar, char* readChar);
char wifi_spi_readChar(void);

int wifi_spi_waitResponseParams(uint8_t cmd, uint8_t numParam, tParam* params);

int wifi_spi_waitResponseCmd(uint8_t cmd, uint8_t numParam, uint8_t* param, uint8_t* param_len);

int wifi_spi_waitResponseData8(uint8_t cmd, uint8_t* param, uint8_t* param_len);

int wifi_spi_waitResponseData16(uint8_t cmd, uint8_t* param, uint16_t* param_len);
/*
int waitResponse(uint8_t cmd, tParam* params, uint8_t* numParamRead, uint8_t maxNumParams);

int waitResponse(uint8_t cmd, uint8_t numParam, uint8_t* param, uint16_t* param_len);
*/
int wifi_spi_waitResponse(uint8_t cmd, uint8_t* numParamRead, uint8_t** params, uint8_t maxNumParams);

void wifi_spi_sendParamLen(uint8_t* param, uint8_t param_len, uint8_t lastParam);

void wifi_spi_sendParamNoLen(uint8_t* param, size_t param_len, uint8_t lastParam); // length field sent to radio is 0

void wifi_spi_sendParamLen8(uint8_t param_len);

void wifi_spi_sendParamLen16(uint16_t param_len);

uint8_t wifi_spi_readParamLen8(uint8_t* param_len);

uint16_t wifi_spi_readParamLen16(uint16_t* param_len);

void wifi_spi_sendBuffer(uint8_t* param, uint16_t param_len, uint8_t lastParam);

void wifi_spi_sendParam(uint16_t param, uint8_t lastParam);

void wifi_spi_sendCmd(uint8_t cmd, uint8_t numParam);

int wifi_spi_available(void);


//extern SpiDrv spiDrv;

#endif