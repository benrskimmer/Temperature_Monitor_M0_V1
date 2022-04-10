/*
  server_drv.h - Library for Arduino Wifi shield.
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

#ifndef Server_Drv_h
#define Server_Drv_h

#include <inttypes.h>
#include "wifi_spi.h"
#include "compiler.h"


typedef enum eProtMode {TCP_MODE, UDP_MODE, TLS_MODE, UDP_MULTICAST_MODE, TLS_BEARSSL_MODE}tProtMode;


// Start server TCP on port specified
void ServerDrv_startServer(uint16_t port, uint8_t sock, uint8_t protMode); //protMode=TCP_MODE

void ServerDrv_startServerIP(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode); //protMode=TCP_MODE

void ServerDrv_startClient(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode); //protMode=TCP_MODE

void ServerDrv_startClientHost(const char* host, uint8_t host_len, uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode); //protMode=TCP_MODE

void ServerDrv_stopClient(uint8_t sock);
                                                                                  
uint8_t ServerDrv_getServerState(uint8_t sock);

uint8_t ServerDrv_getClientState(uint8_t sock);

bool ServerDrv_getData(uint8_t sock, uint8_t *data, uint8_t peek); // peek = 0 by default in c++

bool ServerDrv_getDataBuf(uint8_t sock, uint8_t *data, uint16_t *len);

bool ServerDrv_insertDataBuf(uint8_t sock, const uint8_t *_data, uint16_t _dataLen);

uint16_t ServerDrv_sendData(uint8_t sock, const uint8_t *data, uint16_t len);

bool ServerDrv_sendUdpData(uint8_t sock);

uint16_t ServerDrv_availData(uint8_t sock);

uint8_t ServerDrv_availServer(uint8_t sock);

uint8_t ServerDrv_checkDataSent(uint8_t sock);

uint8_t ServerDrv_getSocket(void);


#endif
