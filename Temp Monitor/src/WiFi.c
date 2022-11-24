/*
 * WiFi.c
 *
 * Created: 8/29/2021 11:50:46 PM
 *  Author: Ben
 */ 
/*
  WiFi.cpp - Library for Arduino Wifi shield.
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

#include "wifi_drv.h"
#include "WiFi.h"


#include "wl_definitions.h"
#include "wl_types.h"
#include "debug.h"
#include "millis.h"


unsigned long _timeout;


// WiFiClass::WiFiClass() : _timeout(50000)
// {
// }

void WiFi_Init(void)
{
	_timeout = 10000;
    wifi_drv_wifiDriverInit();
}

char* WiFi_firmwareVersion()
{
	return wifi_drv_getFwVersion();
}

int WiFi_begin_open(char* ssid)
{
	uint8_t status = WL_IDLE_STATUS;

   if (wifi_drv_wifiSetNetwork(ssid, strlen(ssid)) != WL_FAILURE)
   {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
	   {
		   delay(WL_DELAY_START_CONNECTION);
		   status = wifi_drv_getConnectionStatus();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
	   }
   }else
   {
	   status = WL_CONNECT_FAILED;
   }
   return status;
}

// same as above except asynchronous
void WiFi_begin_open_async(char* ssid)
{
	wifi_drv_wifiSetNetwork(ssid, strlen(ssid));
}

int WiFi_begin_WEP(char* ssid, uint8_t key_idx, char *key)
{
	uint8_t status = WL_IDLE_STATUS;

	// set encryption key
   if (wifi_drv_wifiSetKey(ssid, strlen(ssid), key_idx, key, strlen(key)) != WL_FAILURE)
   {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
	   {
		   delay(WL_DELAY_START_CONNECTION);
		   status = wifi_drv_getConnectionStatus();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
	   }
   }else{
	   status = WL_CONNECT_FAILED;
   }
   return status;
}

int WiFi_begin_passphrase(char* ssid, char *passphrase)
{
	uint8_t status = WL_IDLE_STATUS;

    // set passphrase
    if (wifi_drv_wifiSetPassphrase(ssid, strlen(ssid), passphrase, strlen(passphrase))!= WL_FAILURE)
    {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
 	   {
 		   delay(WL_DELAY_START_CONNECTION);
 		   status = wifi_drv_getConnectionStatus();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
 	   }
    }else{
    	status = WL_CONNECT_FAILED;
    }
    return status;
}

void WiFi_begin_passphrase_async(char* ssid, char *passphrase)
{
	// set passphrase
	wifi_drv_wifiSetPassphrase(ssid, strlen(ssid), passphrase, strlen(passphrase));
}

// uint8_t WiFi_beginAP_open(char *ssid)
// {
// 	return beginAP(ssid, 1);
// }

uint8_t WiFi_beginAP_open(char *ssid, uint8_t channel)
{
	uint8_t status = WL_IDLE_STATUS;

   if (wifi_drv_wifiSetApNetwork(ssid, strlen(ssid), channel) != WL_FAILURE)
   {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
	   {
		   delay(WL_DELAY_START_CONNECTION);
		   status = wifi_drv_getConnectionStatus();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
	   }
   }else
   {
	   status = WL_AP_FAILED;
   }
   return status;
}

// uint8_t WiFi_beginAP(char *ssid, char* passphrase)
// {
// 	return beginAP(ssid, passphrase, 1);
// }

uint8_t WiFi_beginAP_passphrase(char *ssid, char* passphrase, uint8_t channel)
{
	uint8_t status = WL_IDLE_STATUS;

    // set passphrase
    if (wifi_drv_wifiSetApPassphrase(ssid, strlen(ssid), passphrase, strlen(passphrase), channel)!= WL_FAILURE)
    {
	   for (unsigned long start = millis(); (millis() - start) < _timeout;)
	   {
 		   delay(WL_DELAY_START_CONNECTION);
 		   status = wifi_drv_getConnectionStatus();
		   if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
		     break;
		   }
 	   }
    }else{
    	status = WL_AP_FAILED;
    }
    return status;
}

// uint8_t WiFi_beginEnterprise(char* ssid, char* username, char* password)
// {
// 	return beginEnterprise(ssid, username, password, "");
// }
// 
// uint8_t WiFi_beginEnterprise(char* ssid, char* username, char* password, char* identity)
// {
// 	return beginEnterprise(ssid, username, password, identity, "");
// }

uint8_t WiFi_beginEnterprise(char* ssid, char* username, char* password, char* identity, char* ca)
{
	uint8_t status = WL_IDLE_STATUS;

	// set passphrase
	if (wifi_drv_wifiSetEnterprise(0 /*PEAP/MSCHAPv2*/, ssid, strlen(ssid), username, strlen(username), password, strlen(password), identity, strlen(identity), ca, strlen(ca) + 1)!= WL_FAILURE)
	{
		for (unsigned long start = millis(); (millis() - start) < _timeout;)
		{
			delay(WL_DELAY_START_CONNECTION);
			status = wifi_drv_getConnectionStatus();
			if ((status != WL_IDLE_STATUS) && (status != WL_NO_SSID_AVAIL) && (status != WL_SCAN_COMPLETED)) {
				break;
			}
		}
	} else {
		status = WL_CONNECT_FAILED;
	}
	return status;
}

void WiFi_config_ip(ip_addr_t local_ip)
{
	wifi_drv_config(1, IPAddress_uint32_t(local_ip), 0, 0);
}

// void WiFi_config(ip_addr_t local_ip, ip_addr_t dns_server)
// {
// 	wifi_drv_config(1, IPAddress_uint32_t(local_ip), 0, 0);
// 	wifi_drv_setDNS(1, IPAddress_uint32_t(dns_server), 0);
// }
// 
// void WiFi_config(ip_addr_t local_ip, ip_addr_t dns_server, ip_addr_t gateway)
// {
// 	wifi_drv_config(2, IPAddress_uint32_t(local_ip), IPAddress_uint32_t(gateway), 0);
// 	wifi_drv_setDNS(1, IPAddress_uint32_t(dns_server), 0);
// }

void WiFi_config_all(ip_addr_t local_ip, ip_addr_t dns_server, ip_addr_t gateway, ip_addr_t subnet)
{
	wifi_drv_config(3, IPAddress_uint32_t(local_ip), IPAddress_uint32_t(gateway), IPAddress_uint32_t(subnet));
	wifi_drv_setDNS(1, IPAddress_uint32_t(dns_server), 0);
}

void WiFi_setDNS_one(ip_addr_t dns_server1)
{
	wifi_drv_setDNS(1, IPAddress_uint32_t(dns_server1), 0);
}

void WiFi_setDNS_two(ip_addr_t dns_server1, ip_addr_t dns_server2)
{
	wifi_drv_setDNS(2, IPAddress_uint32_t(dns_server1), IPAddress_uint32_t(dns_server2));
}

void WiFi_setHostname(char* name)
{
	wifi_drv_setHostname(name);
}

int WiFi_disconnect()
{
    return wifi_drv_disconnect();
}

void WiFi_end(void)
{
	wifi_drv_wifiDriverDeinit();
}

uint8_t* WiFi_macAddress(uint8_t* mac)
{
	uint8_t* _mac = wifi_drv_getMacAddress();
	memcpy(mac, _mac, WL_MAC_ADDR_LENGTH);
    return mac;
}
   
ip_addr_t WiFi_localIP()
{
	ip_addr_t ret;
	wifi_drv_getIpAddress(&ret);
	return ret;
}

ip_addr_t WiFi_subnetMask()
{
	ip_addr_t ret;
	wifi_drv_getSubnetMask(&ret);
	return ret;
}

ip_addr_t WiFi_gatewayIP()
{
	ip_addr_t ret;
	wifi_drv_getGatewayIP(&ret);
	return ret;
}

char* WiFi_SSID()
{
    return wifi_drv_getCurrentSSID();
}

uint8_t* WiFi_BSSID(uint8_t* bssid)
{
	uint8_t* _bssid = wifi_drv_getCurrentBSSID();
	memcpy(bssid, _bssid, WL_MAC_ADDR_LENGTH);
    return bssid;
}

int32_t WiFi_RSSI()
{
    return wifi_drv_getCurrentRSSI();
}

uint8_t WiFi_encryptionType()
{
    return wifi_drv_getCurrentEncryptionType();
}


int8_t WiFi_scanNetworks()
{
	uint8_t attempts = 10;
	uint8_t numOfNetworks = 0;

	if (wifi_drv_startScanNetworks() == WL_FAILURE)
		return WL_FAILURE;
 	do
 	{
 		delay(2000);
 		numOfNetworks = wifi_drv_getScanNetworks();
 	}
	while (( numOfNetworks == 0)&&(--attempts>0));
	return numOfNetworks;
}

char* WiFi_SSID_From_Scan(uint8_t networkItem)
{
	return wifi_drv_getSSIDNetoworks(networkItem);
}

int32_t WiFi_RSSI_From_Scan(uint8_t networkItem)
{
	return wifi_drv_getRSSINetoworks(networkItem);
}

uint8_t WiFi_encryptionType_From_Scan(uint8_t networkItem)
{
    return wifi_drv_getEncTypeNetowrks(networkItem);
}

uint8_t* WiFi_BSSID_From_Scan(uint8_t networkItem, uint8_t* bssid)
{
	return wifi_drv_getBSSIDNetowrks(networkItem, bssid);
}

uint8_t WiFi_channel_From_Scan(uint8_t networkItem)
{
	return wifi_drv_getChannelNetowrks(networkItem);
}

uint8_t WiFi_status()
{
    return wifi_drv_getConnectionStatus();
}

uint8_t WiFi_reasonCode()
{
	return wifi_drv_getReasonCode();
}

int WiFi_hostByName(char* aHostname, ip_addr_t* aResult)
{
	return wifi_drv_getHostByName(aHostname, aResult);
}

unsigned long WiFi_getTime()
{
	return wifi_drv_getTime();
}

void WiFi_lowPowerMode()
{
	wifi_drv_setPowerMode(1);
}

void WiFi_noLowPowerMode()
{
	wifi_drv_setPowerMode(0);
}

int WiFi_ping(char* hostname, uint8_t ttl)
{
	ip_addr_t ip;

	if (!WiFi_hostByName(hostname, &ip)) {
		return WL_PING_UNKNOWN_HOST;
	}

	return WiFi_ping_ip(ip, ttl);
}

// int WiFi_ping(String &hostname, uint8_t ttl)
// {
// 	return ping(hostname.c_str(), ttl);
// }

int WiFi_ping_ip(ip_addr_t host, uint8_t ttl)
{
	return wifi_drv_ping(IPAddress_uint32_t(host), ttl);
}

void WiFi_setTimeout(unsigned long timeout)
{
	_timeout = timeout;
}
