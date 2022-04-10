/*
 * WiFiServer.c
 *
 * Created: 8/26/2021 1:35:08 AM
 *  Author: Ben
 */ 
/*
  WiFiServer.cpp - Library for Arduino Wifi shield.
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

#include <string.h>
#include <stdint.h>
#include "server_drv.h"
#include "debug.h"


#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"


uint8_t WiFiServer_sock;
uint8_t WiFiServer_lastSock;
uint16_t WiFiServer_port;
//void*    WiFiServer_pcb;


void WiFiServer_Init(uint16_t port)
{
	WiFiServer_sock = NO_SOCKET_AVAIL;
	WiFiServer_lastSock = NO_SOCKET_AVAIL;
    WiFiServer_port = port;
}

void WiFiServer_begin(void)
{
    WiFiServer_sock = ServerDrv_getSocket();
    if (WiFiServer_sock != NO_SOCKET_AVAIL)
    {
        ServerDrv_startServer(WiFiServer_port, WiFiServer_sock, TCP_MODE);
    }
}

//void WiFiServer_available(uint8_t* status)
bool WiFiServer_available(void)
{
    int sock = NO_SOCKET_AVAIL;

    if (WiFiServer_sock != NO_SOCKET_AVAIL) {
      // check previous received client socket
      if (WiFiServer_lastSock != NO_SOCKET_AVAIL) {
          WiFiClient(WiFiServer_lastSock);

          if (WiFiClient_connected() && WiFiClient_available()) {
              sock = WiFiServer_lastSock;
          }
      }

      if (sock == NO_SOCKET_AVAIL) {
          // check for new client socket
          sock = ServerDrv_availServer(WiFiServer_sock);
      }
    }

    if (sock != NO_SOCKET_AVAIL) {
        WiFiClient(sock);

//         if (status != NULL) {
//             *status = WiFiClient_status();
//         }

        WiFiServer_lastSock = sock;

        return true;
    }

    return false;
}

uint8_t WiFiServer_status(void) {
    if (WiFiServer_sock == NO_SOCKET_AVAIL) {
        return CLOSED;
    } else {
        return ServerDrv_getServerState(WiFiServer_sock);
    }
}

size_t WiFiServer_write(char* string) {
	volatile int test = strlen(string);
	return WiFiServer_write_size((uint8_t*) string, strlen(string));
}

size_t WiFiServer_writeln(char* string) {
	size_t written = 0;
	written += WiFiServer_write(string);
	char newline_char[2] = "\n";
	written += WiFiServer_write(newline_char);
	return written;
}

size_t WiFiServer_write_byte(uint8_t b)
{
    return WiFiServer_write_size(&b, 1);
}

size_t WiFiServer_write_size(uint8_t *buffer, size_t size)
{
    if (size==0)
    {
//        setWriteError();
        return 0;
    }

    size_t written = ServerDrv_sendData(WiFiServer_sock, buffer, size);
    if (!written)
    {
//        setWriteError();
        return 0;
    }

    if (!ServerDrv_checkDataSent(WiFiServer_sock))
    {
//        setWriteError();
        return 0;
    }

    return written;
}
