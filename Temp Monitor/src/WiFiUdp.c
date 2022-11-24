/*
 * WiFiUdp.c
 *
 * Created: 6/27/2022 1:44:14 AM
 *  Author: Ben
 */ 
/*
  WiFiUdp.cpp - Library for Arduino Wifi shield.
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

#include "debug.h"
#include "wifi_spi.h"
#include "server_drv.h"
#include "wifi_drv.h"
#include "WiFiSocketBuffer.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "WiFiClient.h"
#include "WiFiServer.h"

// Private variables
uint8_t _sock = NO_SOCKET_AVAIL;  // socket ID for Wiz5100
uint16_t _port; // local port to listen on
int _parsed = 0;

/* Constructor */
//UDP_WiFiUDP() : _sock(NO_SOCKET_AVAIL), _parsed(0) {}

/* Start WiFiUDP socket, listening at local port PORT */
uint8_t UDP_begin(uint16_t port) {
    if (_sock != NO_SOCKET_AVAIL)
    {
        UDP_stop();
    }

    uint8_t sock = ServerDrv_getSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        ServerDrv_startServer(port, sock, UDP_MODE);
        _sock = sock;
        _port = port;
        _parsed = 0;
        return 1;
    }
    return 0;
}

uint8_t UDP_beginMulticast(ip_addr_t ip, uint16_t port) {
    if (_sock != NO_SOCKET_AVAIL)
    {
        UDP_stop();
    }

    uint8_t sock = ServerDrv_getSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        ServerDrv_startServerIP(IPAddress_uint32_t(ip), port, sock, UDP_MULTICAST_MODE);
        _sock = sock;
        _port = port;
        _parsed = 0;
        return 1;
    }
    return 0;
}

/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int UDP_available(void) {
	 return _parsed;
}

/* Release any resources being used by this WiFiUDP instance */
void UDP_stop(void)
{
	  if (_sock == NO_SOCKET_AVAIL)
	    return;

	  ServerDrv_stopClient(_sock);

	  WiFiSocketBuffer_close(_sock);
	  _sock = NO_SOCKET_AVAIL;
}

int UDP_beginPacket(const char *host, uint16_t port)
{
	// Look up the host first
	int ret = 0;
	ip_addr_t remote_addr;
	if (WiFi_hostByName(host, &remote_addr))
	{
		return UDP_beginPacketIP(remote_addr, port);
	}
	return ret;
}

int UDP_beginPacketIP(ip_addr_t ip, uint16_t port)
{
  if (_sock == NO_SOCKET_AVAIL)
	  _sock = ServerDrv_getSocket();
  if (_sock != NO_SOCKET_AVAIL)
  {
	  ServerDrv_startClient(IPAddress_uint32_t(ip), port, _sock, UDP_MODE);
	  return 1;
  }
  return 0;
}

int UDP_endPacket(void)
{
	return ServerDrv_sendUdpData(_sock);
}

size_t UDP_write_byte(uint8_t byte)
{
  return UDP_write(&byte, 1);
}

size_t UDP_write(const uint8_t *buffer, size_t size)
{
	ServerDrv_insertDataBuf(_sock, buffer, size);
	return size;
}

int UDP_parsePacket(void)
{
	while (_parsed--)
	{
	  // discard previously parsed packet data
	  uint8_t b;

	  WiFiSocketBuffer_read(_sock, &b, sizeof(b));
	}

	_parsed = ServerDrv_availData(_sock);

	return _parsed;
}

int UDP_read_byte(void)
{
  if (_parsed < 1)
  {
    return -1;
  }

  uint8_t b;

  WiFiSocketBuffer_read(_sock, &b, sizeof(b));
  _parsed--;

  return b;
}

int UDP_read(unsigned char* buffer, size_t len)
{
  if (_parsed < 1)
  {
    return 0;
  }

  int result = WiFiSocketBuffer_read(_sock, buffer, len);

  if (result > 0)
  {
    _parsed -= result;
  }

  return result;
}

// int UDP_read(char* buffer, size_t len)
// {
// 	return read((unsigned char*)buffer, len);
// }

int UDP_peek(void)
{
  if (_parsed < 1)
  {
    return -1;
  }

  return WiFiSocketBuffer_peek(_sock);
}

void UDP_flush(void)
{
  // TODO: a real check to ensure transmission has been completed
}

ip_addr_t UDP_remoteIP(void)
{
	uint8_t _remoteIp[4] = {0};
	uint8_t _remotePort[2] = {0};

	wifi_drv_getRemoteData(_sock, _remoteIp, _remotePort);
	ip_addr_t ip;
	IPAddress_From_UVAR8(&ip, _remoteIp);
	return ip;
}

uint16_t  UDP_remotePort(void)
{
	uint8_t _remoteIp[4] = {0};
	uint8_t _remotePort[2] = {0};

	wifi_drv_getRemoteData(_sock, _remoteIp, _remotePort);
	uint16_t port = (_remotePort[0]<<8)+_remotePort[1];
	return port;
}