/*
  WiFiServer.h - Library for Arduino Wifi shield.
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

#ifndef wifiserver_h
#define wifiserver_h

#include "wl_definitions.h"
//#include "Server.h"

//class WiFiServer : public Server {

  void WiFiServer_Init(uint16_t);
  bool WiFiServer_available(void); //uint8_t* status = NULL
  void WiFiServer_begin(void);
  size_t WiFiServer_write(char* string);
  size_t WiFiServer_writeln(char* string);
  size_t WiFiServer_write_byte(uint8_t);
  size_t WiFiServer_write_size(uint8_t *buf, size_t size);
  uint8_t WiFiServer_status(void);


#endif
