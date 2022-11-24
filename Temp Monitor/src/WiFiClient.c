/*
 * WiFiClient.c
 *
 * Created: 8/23/2021 1:24:18 AM
 *  Author: Ben
 */

//extern "C" {
#include <stdio.h>
#include "wl_definitions.h"
#include "wl_types.h"
#include "string.h"
#include "debug.h"
#include "millis.h"
//}


#include "server_drv.h"
#include "wifi_drv.h"
#include "WiFiSocketBuffer.h"

#include "WiFi.h"
#include "WiFiClient.h"


uint16_t _srcport;
uint8_t _sock;
uint16_t  _socket;


uint16_t WiFiClient__srcport = 1024;

// WiFiClient() : _sock(NO_SOCKET_AVAIL) {
// }

void WiFiClient(uint8_t sock) {
	_sock = sock;
}

int WiFiClient_connectHost(char* host, uint16_t port) {
	ip_addr_t remote_addr;
	if (WiFi_hostByName(host, &remote_addr))
	{
		printf("Resolved host address is: ");
		IP_PrintAddress(&remote_addr);
		printf("\n");
		return WiFiClient_connectIP(remote_addr, port);
	}
	return 0;
}

int WiFiClient_connectIP(ip_addr_t ip, uint16_t port) {
    if (_sock != NO_SOCKET_AVAIL)
    {
      WiFiClient_stop();
    }

    _sock = ServerDrv_getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
    	ServerDrv_startClient(IPAddress_uint32_t(ip), port, _sock, TCP_MODE);

    	unsigned long start = millis();

    	// wait 4 second for the connection to close
    	while (!WiFiClient_connected() && millis() - start < 10000)
    		delay(1);

    	if (!WiFiClient_connected())
       	{
    		return 0;
    	}
    } else {
    	printf("No Socket available\n");
    	return 0;
    }
    return 1;
}

int WiFiClient_connectSSLIP(ip_addr_t ip, uint16_t port)
{
    if (_sock != NO_SOCKET_AVAIL)
    {
      WiFiClient_stop();
    }

    _sock = ServerDrv_getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
      ServerDrv_startClient(IPAddress_uint32_t(ip), port, _sock, TLS_MODE);

      unsigned long start = millis();

      // wait 4 second for the connection to close
      while (!WiFiClient_connected() && millis() - start < 10000)
        delay(1);

      if (!WiFiClient_connected())
        {
        return 0;
      }
    } else {
      printf("No Socket available\n");
      return 0;
    }
    return 1;
}

int WiFiClient_connectSSLHost(char *host, uint16_t port)
{
    if (_sock != NO_SOCKET_AVAIL)
    {
      WiFiClient_stop();
    }

    _sock = ServerDrv_getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
      ServerDrv_startClientHost(host, strlen(host), (uint32_t)0, port, _sock, TLS_MODE);

      unsigned long start = millis();

      // wait 4 second for the connection to close
      while (!WiFiClient_connected() && millis() - start < 10000)
        delay(1);

      if (!WiFiClient_connected())
        {
        return 0;
      }
    } else {
      printf("No Socket available\n");
      return 0;
    }
    return 1;
}

/*
int WiFiClient_connectBearSSLIP(ip_addr_t ip, uint16_t port)
{
    if (_sock != NO_SOCKET_AVAIL)
    {
      WiFiClient_stop();
    }

    _sock = ServerDrv_getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
      ServerDrv_startClient(IPAddress_uint32_t(ip), port, _sock, TLS_BEARSSL_MODE);

      unsigned long start = millis();

      // wait 4 second for the connection to close
      while (!WiFiClient_connected() && millis() - start < 10000)
        delay(1);

      if (!WiFiClient_connected())
        {
        return 0;
      }
    } else {
      printf("No Socket available\n");
      return 0;
    }
    return 1;
}

int WiFiClient_connectBearSSLHost(char *host, uint16_t port)
{
    if (_sock != NO_SOCKET_AVAIL)
    {
      WiFiClient_stop();
    }

    _sock = ServerDrv_getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
      ServerDrv_startClientHost(host, strlen(host), (uint32_t)0, port, _sock, TLS_BEARSSL_MODE);

      unsigned long start = millis();

      // wait 4 second for the connection to close
      while (!WiFiClient_connected() && millis() - start < 10000)
//        delay(1);

      if (!WiFiClient_connected())
        {
        return 0;
      }
    } else {
      printf("No Socket available\n");
      return 0;
    }
    return 1;
}
*/

size_t WiFiClient_write_byte(uint8_t b) {
	  return WiFiClient_write_size(&b, 1);
}

size_t WiFiClient_write(char* string) {
	//volatile int test = strlen(string);
	return WiFiClient_write_size((uint8_t*) string, strlen(string));
}

size_t WiFiClient_writeln(char* string) {
	size_t written = 0;
	written += WiFiClient_write(string);
	char newline_char[2] = "\n";
	written += WiFiClient_write(newline_char);
	return written;
}

size_t WiFiClient_write_size(uint8_t *buf, size_t size) {
  if (_sock == NO_SOCKET_AVAIL)
  {
//	  setWriteError();
	  return 0;
  }
  if (size==0)
  {
//	  setWriteError();
      return 0;
  }

  size_t written = ServerDrv_sendData(_sock, buf, size);
  if (!written)
  {
//	  setWriteError();
      return 0;
  }
  if (!ServerDrv_checkDataSent(_sock))
  {
//	  setWriteError();
      return 0;
  }

  return written;
}

int WiFiClient_available() {
  if (_sock != 255)
  {
      return WiFiSocketBuffer_available(_sock);
  }
   
  return 0;
}

int WiFiClient_read_byte() {
  if (!WiFiClient_available())
  {
    return -1;
  }

  uint8_t b;

  WiFiSocketBuffer_read(_sock, &b, sizeof(b));

  return b;
}


int WiFiClient_read_size(uint8_t* buf, size_t size) {
  return  WiFiSocketBuffer_read(_sock, buf, size);
}

int WiFiClient_peek() {
  return WiFiSocketBuffer_peek(_sock);
}

void WiFiClient_flush() {
  // TODO: a real check to ensure transmission has been completed
}

void WiFiClient_stop() {

  if (_sock == 255)
    return;

  ServerDrv_stopClient(_sock);
  //ServerDrv_stopClient(0);

  int count = 0;
  // wait maximum 5 secs for the connection to close
  while (WiFiClient_status() != CLOSED && ++count < 50)
    delay(100);

  WiFiSocketBuffer_close(_sock);
  //WiFiSocketBuffer_close(0);
  _sock = 255;
}

uint8_t WiFiClient_connected() {

  if (_sock == 255) {
    return 0;
  } else if (WiFiClient_available()) {
    return 1;
  } else {
    uint8_t s = WiFiClient_status();

    uint8_t result =  !(s == LISTEN || s == CLOSED || s == FIN_WAIT_1 ||
                      s == FIN_WAIT_2 || s == TIME_WAIT ||
                      s == SYN_SENT || s== SYN_RCVD ||
                      (s == CLOSE_WAIT));

    if (result == 0) {
      WiFiSocketBuffer_close(_sock);
      _sock = 255;
    }

    return result;
  }
}

uint8_t WiFiClient_status() {
    if (_sock == 255) {
    return CLOSED;
  } else {
    return ServerDrv_getClientState(_sock);
  }
}

bool WiFiClient_bool() {
  return _sock != 255;
}

ip_addr_t  WiFiClient_remoteIP()
{
  uint8_t _remoteIp[4] = {0};
  uint8_t _remotePort[2] = {0};

  wifi_drv_getRemoteData(_sock, _remoteIp, _remotePort);
  ip_addr_t ip;
  IPAddress_From_UVAR8(&ip, _remoteIp);
  return ip;
}

uint16_t  WiFiClient_remotePort()
{
  uint8_t _remoteIp[4] = {0};
  uint8_t _remotePort[2] = {0};

  wifi_drv_getRemoteData(_sock, _remoteIp, _remotePort);
  uint16_t port = (_remotePort[0]<<8)+_remotePort[1];
  return port;
}