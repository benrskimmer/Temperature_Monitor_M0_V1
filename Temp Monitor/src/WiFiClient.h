/*
  WiFiClient.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

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

#ifndef wificlient_h
#define wificlient_h
//#include "Arduino.h"	
//#include "Print.h"
//#include "Client.h"
#include "IPAddress.h"
#include "compiler.h"

//class WiFiClient : public Client {

//public:
  //WiFiClient();
void WiFiClient(uint8_t sock);

uint8_t WiFiClient_status();
int WiFiClient_connectIP(ip_addr_t ip, uint16_t port);
int WiFiClient_connectHost(char *host, uint16_t port);
int WiFiClient_connectSSLIP(ip_addr_t ip, uint16_t port);
int WiFiClient_connectSSLHost(char *host, uint16_t port);
// int WiFiClient_connectBearSSLIP(ip_addr_t ip, uint16_t port);
// int WiFiClient_connectBearSSLHost(char *host, uint16_t port);
size_t WiFiClient_write(char* string);
size_t WiFiClient_writeln(char* string);
size_t WiFiClient_write_byte(uint8_t);
size_t WiFiClient_write_size(uint8_t *buf, size_t size);
int WiFiClient_available();
int WiFiClient_read_byte();
int WiFiClient_read_size(uint8_t *buf, size_t size);
int WiFiClient_peek();
void WiFiClient_flush();
void WiFiClient_stop();
uint8_t WiFiClient_connected();
bool WiFiClient_bool();

ip_addr_t WiFiClient_remoteIP();
uint16_t WiFiClient_remotePort();

//   friend class WiFiServer;
//   friend class WiFiDrv;

//  using Print::write;

//private:
//   uint16_t _srcport;
//   uint8_t _sock;   //not used
//   uint16_t  _socket;
//};

#endif
