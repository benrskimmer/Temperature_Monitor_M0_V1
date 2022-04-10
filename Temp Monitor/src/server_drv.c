/*
 * server_drv.c
 *
 * Created: 8/26/2021 1:43:09 AM
 *  Author: Ben
 */ 

/*
  server_drv.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

//#define _DEBUG_

#include "server_drv.h"

//#include "Arduino.h"
#include "spi_drv.h"

#include "wl_types.h"
#include "debug.h"
#include "millis.h"


// Start server TCP on port specified
void ServerDrv_startServer(uint16_t port, uint8_t sock, uint8_t protMode)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(START_SERVER_TCP_CMD, PARAM_NUMS_3);
    wifi_spi_sendParam(port, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&sock, 1, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&protMode, 1, LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(START_SERVER_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
}

void ServerDrv_startServerIP(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(START_SERVER_TCP_CMD, PARAM_NUMS_4);
    wifi_spi_sendParamLen((uint8_t*)&ipAddress, sizeof(ipAddress), NO_LAST_PARAM);
    wifi_spi_sendParam(port, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&sock, 1, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&protMode, 1, LAST_PARAM);

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(START_SERVER_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
}

// Start server TCP on port specified
void ServerDrv_startClient(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_4);
    wifi_spi_sendParamLen((uint8_t*)&ipAddress, sizeof(ipAddress), NO_LAST_PARAM);
    wifi_spi_sendParam(port, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&sock, 1, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&protMode, 1, LAST_PARAM);

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
}

void ServerDrv_startClientHost(const char* host, uint8_t host_len, uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_5);
    wifi_spi_sendParamLen((uint8_t*)host, host_len, NO_LAST_PARAM);
    wifi_spi_sendParamLen((uint8_t*)&ipAddress, sizeof(ipAddress), NO_LAST_PARAM);
    wifi_spi_sendParam(port, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&sock, 1, NO_LAST_PARAM);
    wifi_spi_sendParamLen(&protMode, 1, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 17 + host_len;
    while (commandSize % 4) {
        wifi_spi_readChar();
        commandSize++;
    }

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(START_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();  
}

// Start server TCP on port specified
void ServerDrv_stopClient(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(STOP_CLIENT_TCP_CMD, PARAM_NUMS_1);
    wifi_spi_sendParamLen(&sock, 1, LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(STOP_CLIENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
}


uint8_t ServerDrv_getServerState(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1);
    wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(GET_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
   return _data;
}

uint8_t ServerDrv_getClientState(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(GET_CLIENT_STATE_TCP_CMD, PARAM_NUMS_1);
    wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseCmd(GET_CLIENT_STATE_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
   return _data;
}

uint16_t ServerDrv_availData(uint8_t sock)
{
    if (!wifi_spi_available()) {
        return 0;
    }

	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1);
    wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
	uint16_t len = 0;

    wifi_spi_waitResponseCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1, (uint8_t*)&len,  &_dataLen);

    wifi_spi_spiSlaveDeselect();

    return len;
}

uint8_t ServerDrv_availServer(uint8_t sock)
{
    if (!wifi_spi_available()) {
        return 255;
    }

    WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1);
    wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _dataLen = 0;
    uint16_t socket = 0;

    wifi_spi_waitResponseCmd(AVAIL_DATA_TCP_CMD, PARAM_NUMS_1, (uint8_t*)&socket,  &_dataLen);

    wifi_spi_spiSlaveDeselect();

    return socket;
}

bool ServerDrv_getData(uint8_t sock, uint8_t *data, uint8_t peek)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(GET_DATA_TCP_CMD, PARAM_NUMS_2);
    wifi_spi_sendParamLen(&sock, sizeof(sock), NO_LAST_PARAM);
    wifi_spi_sendParam(peek, LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseData8(GET_DATA_TCP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
    if (_dataLen!=0)
    {
        *data = _data;
        return true;
    }
    return false;
}

bool ServerDrv_getDataBuf(uint8_t sock, uint8_t *_data, uint16_t *_dataLen)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(GET_DATABUF_TCP_CMD, PARAM_NUMS_2);
    wifi_spi_sendBuffer(&sock, sizeof(sock), NO_LAST_PARAM);
    wifi_spi_sendBuffer((uint8_t *)_dataLen, sizeof(*_dataLen), LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    if (!wifi_spi_waitResponseData16(GET_DATABUF_TCP_CMD, _data, _dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
    if (*_dataLen!=0)
    {
        return true;
    }
    return false;
}

bool ServerDrv_insertDataBuf(uint8_t sock, const uint8_t *data, uint16_t _len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(INSERT_DATABUF_CMD, PARAM_NUMS_2);
    wifi_spi_sendBuffer(&sock, sizeof(sock), NO_LAST_PARAM);
    wifi_spi_sendBuffer((uint8_t *)data, _len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 9 + _len;
    while (commandSize % 4) {
        wifi_spi_readChar();
        commandSize++;
    }

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseData8(INSERT_DATABUF_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}

bool ServerDrv_sendUdpData(uint8_t sock)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(SEND_DATA_UDP_CMD, PARAM_NUMS_1);
    wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

    // pad to multiple of 4
    wifi_spi_readChar();
    wifi_spi_readChar();

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseData8(SEND_DATA_UDP_CMD, &_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();
    if (_dataLen!=0)
    {
        return (_data == 1);
    }
    return false;
}


uint16_t ServerDrv_sendData(uint8_t sock, const uint8_t *data, uint16_t len)
{
	WAIT_FOR_SLAVE_SELECT();
    // Send Command
    wifi_spi_sendCmd(SEND_DATA_TCP_CMD, PARAM_NUMS_2);
    wifi_spi_sendBuffer(&sock, sizeof(sock), NO_LAST_PARAM);
    wifi_spi_sendBuffer((uint8_t *)data, len, LAST_PARAM);

    // pad to multiple of 4
    int commandSize = 9 + len;
    while (commandSize % 4) {
        wifi_spi_readChar();
        commandSize++;
    }

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint16_t _data = 0;
    uint8_t _dataLen = 0;
    if (!wifi_spi_waitResponseData8(SEND_DATA_TCP_CMD, (uint8_t*)&_data, &_dataLen))
    {
        WARN("error waitResponse");
    }
    wifi_spi_spiSlaveDeselect();

    return _data;
}


uint8_t ServerDrv_checkDataSent(uint8_t sock)
{
	const uint16_t TIMEOUT_DATA_SENT = 25;
    uint16_t timeout = 0;
	uint8_t _data = 0;
	uint8_t _dataLen = 0;

	do {
		WAIT_FOR_SLAVE_SELECT();
		// Send Command
		wifi_spi_sendCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1);
		wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

        // pad to multiple of 4
        wifi_spi_readChar();
        wifi_spi_readChar();

        wifi_spi_spiSlaveDeselect();
        //Wait the reply elaboration
        wifi_spi_waitForSlaveReady();
        wifi_spi_spiSlaveSelect();

		// Wait for reply
		if (!wifi_spi_waitResponseCmd(DATA_SENT_TCP_CMD, PARAM_NUMS_1, &_data, &_dataLen))
		{
			WARN("error waitResponse isDataSent");
		}
		wifi_spi_spiSlaveDeselect();

		if (_data) timeout = 0;
		else{
			++timeout;
			delay(100);
		}

	}while((_data==0)&&(timeout<TIMEOUT_DATA_SENT));
    return (timeout==TIMEOUT_DATA_SENT)?0:1;
}

uint8_t ServerDrv_getSocket(void)
{
    WAIT_FOR_SLAVE_SELECT();

    // Send Command
    wifi_spi_sendCmd(GET_SOCKET_CMD, PARAM_NUMS_0);

    wifi_spi_spiSlaveDeselect();
    //Wait the reply elaboration
    wifi_spi_waitForSlaveReady();
    wifi_spi_spiSlaveSelect();

    // Wait for reply
    uint8_t _data = -1;
    uint8_t _dataLen = 0;
    wifi_spi_waitResponseCmd(GET_SOCKET_CMD, PARAM_NUMS_1, &_data, &_dataLen);

    wifi_spi_spiSlaveDeselect();

    return _data;
}

/*ServerDrv serverDrv;*/
