/*
  wifi_drv.h - Library for Arduino Wifi shield.
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

#ifndef WiFi_Drv_h
#define WiFi_Drv_h

#include <inttypes.h>
#include "wifi_spi.h"
#include "IPAddress.h"
//#include "WiFiUdp.h"
//#include "WiFiClient.h"

// Key index length
#define KEY_IDX_LEN     1
// 100 msecs of delay to have the connection established
#define WL_DELAY_START_CONNECTION 100
// firmware version string length
#define WL_FW_VER_LENGTH 6

/*
    * Driver initialization
    */
void wifi_drv_wifiDriverInit(void);

void wifi_drv_wifiDriverDeinit(void);

/*
    * Set the desired network which the connection manager should try to
    * connect to.
    *
    * The ssid of the desired network should be specified.
    *
    * param ssid: The ssid of the desired network.
    * param ssid_len: Lenght of ssid string.
    * return: WL_SUCCESS or WL_FAILURE
	*/
int8_t wifi_drv_wifiSetNetwork(const char* ssid, uint8_t ssid_len);

/* Start Wifi connection with passphrase
    * the most secure supported mode will be automatically selected
    *
    * param ssid: Pointer to the SSID string.
    * param ssid_len: Lenght of ssid string.
    * param passphrase: Passphrase. Valid characters in a passphrase
    *        must be between ASCII 32-126 (decimal).
    * param len: Lenght of passphrase string.
    * return: WL_SUCCESS or WL_FAILURE
    */
int8_t wifi_drv_wifiSetPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len);

/* Start Wifi connection with WEP encryption.
    * Configure a key into the device. The key type (WEP-40, WEP-104)
    * is determined by the size of the key (5 bytes for WEP-40, 13 bytes for WEP-104).
    *
    * param ssid: Pointer to the SSID string.
    * param ssid_len: Lenght of ssid string.
    * param key_idx: The key index to set. Valid values are 0-3.
    * param key: Key input buffer.
    * param len: Lenght of key string.
    * return: WL_SUCCESS or WL_FAILURE
    */
int8_t wifi_drv_wifiSetKey(const char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len);

//int8_t wifi_drv_wifiSetApNetwork(const char* ssid, uint8_t ssid_len);
//int8_t wifi_drv_wifiSetApPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len);

/* Set ip configuration disabling dhcp client
    *
    * param validParams: set the number of parameters that we want to change
    * 					 i.e. validParams = 1 means that we'll change only ip address
    * 					 	  validParams = 3 means that we'll change ip address, gateway and netmask
    * param local_ip: 	ip configuration
    * param gateway: 	gateway configuration
    * param subnet: 	subnet mask configuration
    */
void wifi_drv_config(uint8_t validParams, uint32_t local_ip, uint32_t gateway, uint32_t subnet);

/* Set DNS ip configuration
        *
        * param validParams: set the number of parameters that we want to change
        * 					 i.e. validParams = 1 means that we'll change only dns_server1
        * 					 	  validParams = 2 means that we'll change dns_server1 and dns_server2
        * param dns_server1: DNS server1 configuration
        * param dns_server2: DNS server2 configuration
        */
void wifi_drv_setDNS(uint8_t validParams, uint32_t dns_server1, uint32_t dns_server2);

void wifi_drv_setHostname(const char* hostname);

/*
    * Disconnect from the network
    *
    * return: WL_SUCCESS or WL_FAILURE
    */
int8_t wifi_drv_disconnect(void);

uint8_t wifi_drv_getReasonCode(void);

/*
    * Disconnect from the network
    *
    * return: one value of wl_status_t enum
    */
uint8_t wifi_drv_getConnectionStatus(void);

/*
    * Get the interface MAC address.
    *
    * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
    */
uint8_t* wifi_drv_getMacAddress(void);

/*
    * Get the interface IP address.
    *
    * return: copy the ip address value in IPAddress object
    */
void wifi_drv_getIpAddress(ip_addr_t* ip);

/*
    * Get the interface subnet mask address.
    *
    * return: copy the subnet mask address value in IPAddress object
    */
void wifi_drv_getSubnetMask(ip_addr_t* mask);

/*
    * Get the gateway ip address.
    *
    * return: copy the gateway ip address value in IPAddress object
    */
void wifi_drv_getGatewayIP(ip_addr_t* ip);

/*
    * Return the current SSID associated with the network
    *
    * return: ssid string
    */
const char* wifi_drv_getCurrentSSID(void);

/*
    * Return the current BSSID associated with the network.
    * It is the MAC address of the Access Point
    *
    * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
    */
uint8_t* wifi_drv_getCurrentBSSID(void);

/*
    * Return the current RSSI /Received Signal Strength in dBm)
    * associated with the network
    *
    * return: signed value
    */
int32_t wifi_drv_getCurrentRSSI(void);

/*
    * Return the Encryption Type associated with the network
    *
    * return: one value of wl_enc_type enum
    */
uint8_t wifi_drv_getCurrentEncryptionType(void);

/*
    * Start scan WiFi networks available
    *
    * return: Number of discovered networks
    */
int8_t wifi_drv_startScanNetworks(void);

/*
    * Get the networks available
    *
    * return: Number of discovered networks
    */
uint8_t wifi_drv_getScanNetworks(void);

/*
    * Return the SSID discovered during the network scan.
    *
    * param networkItem: specify from which network item want to get the information
	*
    * return: ssid string of the specified item on the networks scanned list
    */
const char* wifi_drv_getSSIDNetoworks(uint8_t networkItem);

/*
    * Return the RSSI of the networks discovered during the scanNetworks
    *
    * param networkItem: specify from which network item want to get the information
	*
    * return: signed value of RSSI of the specified item on the networks scanned list
    */
int32_t wifi_drv_getRSSINetoworks(uint8_t networkItem);

/*
    * Return the encryption type of the networks discovered during the scanNetworks
    *
    * param networkItem: specify from which network item want to get the information
	*
    * return: encryption type (enum wl_enc_type) of the specified item on the networks scanned list
*/
uint8_t wifi_drv_getEncTypeNetowrks(uint8_t networkItem);

uint8_t* wifi_drv_getBSSIDNetowrks(uint8_t networkItem, uint8_t* bssid);

uint8_t wifi_drv_getChannelNetowrks(uint8_t networkItem);

/*
    * Resolve the given hostname to an IP address.
    * param aHostname: Name to be resolved
    * param aResult: IPAddress structure to store the returned IP address
    * result: 1 if aIPAddrString was successfully converted to an IP address,
    *          else error code
    */
int wifi_drv_getHostByName(const char* aHostname, ip_addr_t* aResult);

/*
    * Get the firmware version
    * result: version as string with this format a.b.c
    */
const char* wifi_drv_getFwVersion(void);

uint32_t wifi_drv_getTime(void);

/*
    * Get remote Data information on UDP socket
    */
void wifi_drv_getRemoteData(uint8_t sock, uint8_t *ip, uint8_t *port);

void wifi_drv_setPowerMode(uint8_t mode);

int8_t wifi_drv_wifiSetApNetwork(const char* ssid, uint8_t ssid_len, uint8_t channel);
int8_t wifi_drv_wifiSetApPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len, uint8_t channel);
int8_t wifi_drv_wifiSetEnterprise(uint8_t eapType,
                                const char* ssid, uint8_t ssid_len,
                                const char *username, const uint8_t username_len,
                                const char *password, const uint8_t password_len,
                                const char *identity, const uint8_t identity_len,
                                const char* ca_cert, uint16_t ca_cert_len);


int16_t wifi_drv_ping(uint32_t ipAddress, uint8_t ttl);

void wifi_drv_debug(uint8_t on);
float wifi_drv_getTemperature();
void wifi_drv_pinMode(uint8_t pin, uint8_t mode);
void wifi_drv_digitalWrite(uint8_t pin, uint8_t value);
void wifi_drv_analogWrite(uint8_t pin, uint8_t value);

int8_t wifi_drv_downloadFile(const char* url, uint8_t url_len, const char *filename, uint8_t filename_len);
int8_t wifi_drv_downloadOTA(const char* url, uint8_t url_len);
int8_t wifi_drv_renameFile(const char * old_file_name, uint8_t const old_file_name_len, const char * new_file_name, uint8_t const new_file_name_len);

int8_t wifi_drv_fileOperation(uint8_t operation, const char *filename, uint8_t filename_len, uint32_t offset, uint8_t* buffer, uint32_t len);

// int8_t wifi_drv_readFile(const char *filename, uint8_t filename_len, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) {
//     return fileOperation(READ_FILE, filename, filename_len, offset, buffer, buffer_len);
// };
// int8_t wifi_drv_writeFile(const char *filename, uint8_t filename_len, uint32_t offset, uint8_t* buffer, uint32_t buffer_len) {
//     return fileOperation(WRITE_FILE, filename, filename_len, offset, buffer, buffer_len);
// };
// int8_t wifi_drv_deleteFile(const char *filename, uint8_t filename_len)  {
//     return fileOperation(DELETE_FILE, filename, filename_len, 0, NULL, 0);
// };
// int8_t wifi_drv_existsFile(const char *filename, uint8_t filename_len, uint32_t* len)  {
//     int32_t length = 0;
//     fileOperation(EXISTS_FILE, filename, filename_len, 0, (uint8_t*)&length, sizeof(length));
//     *len = length;
//     return length >= 0;
// };

void wifi_drv_applyOTA(void);
// 
// friend class WiFiUDP;
// friend class WiFiClient;


//extern WiFiDrv wiFiDrv;

#endif
