/*
  WiFi.h - Library for Arduino Wifi shield.
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

#ifndef WiFi_h
#define WiFi_h

#define WIFI_FIRMWARE_LATEST_VERSION "1.4.2"

#include <inttypes.h>

// extern "C" {
#include "wl_definitions.h"
#include "wl_types.h"
// }

#include "IPAddress.h"
#include "WiFiClient.h"
#include "WiFiSSLClient.h"
#include "WiFiServer.h"
//#include "WiFiStorage.h"
#include "wifi_drv.h"
#include "WiFiUdp.h"

// class WiFiClass
// {
// private:

    
//public:
    void WiFi_Init(void);

    /*
     * Get firmware version
     */
    static char* WiFi_firmwareVersion();


    /* Start Wifi connection for OPEN networks
     *
     * param ssid: Pointer to the SSID string.
     */
    int WiFi_begin_open(char* ssid);
	
	// same as above except asynchronous
	void WiFi_begin_open_async(char* ssid);

    /* Start Wifi connection with WEP encryption.
     * Configure a key into the device. The key type (WEP-40, WEP-104)
     * is determined by the size of the key (5 bytes for WEP-40, 13 bytes for WEP-104).
     *
     * param ssid: Pointer to the SSID string.
     * param key_idx: The key index to set. Valid values are 0-3.
     * param key: Key input buffer.
     */
    int WiFi_begin_WEP(char* ssid, uint8_t key_idx, char* key);

    /* Start Wifi connection with passphrase
     * the most secure supported mode will be automatically selected
     *
     * param ssid: Pointer to the SSID string.
     * param passphrase: Passphrase. Valid characters in a passphrase
     *        must be between ASCII 32-126 (decimal).
     */
    int WiFi_begin_passphrase(char* ssid, char *passphrase);
	
	// same as above except asynchronous, wl_status must be polled afterwards to ensure connection
	void WiFi_begin_passphrase_async(char* ssid, char *passphrase);

//    uint8_t WiFi_beginAP_open(char *ssid);
    uint8_t WiFi_beginAP_open(char *ssid, uint8_t channel);
//    uint8_t WiFi_beginAP_passphrase(char *ssid, char* passphrase);
    uint8_t WiFi_beginAP_passphrase(char *ssid, char* passphrase, uint8_t channel);

//     uint8_t WiFi_beginEnterprise(char* ssid, char* username, char* password);
//     uint8_t WiFi_beginEnterprise(char* ssid, char* username, char* password, char* identity);
    uint8_t WiFi_beginEnterprise(char* ssid, char* username, char* password, char* identity, char* ca); // use "" for identity and ca if not available

    /* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip: 	Static ip configuration
        */
    void WiFi_config_ip(ip_addr_t local_ip);

    /* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip: 	Static ip configuration
	* param dns_server:     IP configuration for DNS server 1
        */
/*    void WiFi_config(ip_addr_t local_ip, ip_addr_t dns_server);*/

    /* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip: 	Static ip configuration
	* param dns_server:     IP configuration for DNS server 1
        * param gateway : 	Static gateway configuration
        */
/*    void WiFi_config(ip_addr_t local_ip, ip_addr_t dns_server, ip_addr_t gateway);*/

    /* Change Ip configuration settings disabling the dhcp client
        *
        * param local_ip: 	Static ip configuration
	* param dns_server:     IP configuration for DNS server 1
        * param gateway: 	Static gateway configuration
        * param subnet:		Static Subnet mask
        */
    void WiFi_config_all(ip_addr_t local_ip, ip_addr_t dns_server, ip_addr_t gateway, ip_addr_t subnet); // gateway and subnet should be set to 0 to default them

    /* Change DNS Ip configuration
     *
     * param dns_server1: ip configuration for DNS server 1
     */
    void WiFi_setDNS_one(ip_addr_t dns_server1);

    /* Change DNS Ip configuration
     *
     * param dns_server1: ip configuration for DNS server 1
     * param dns_server2: ip configuration for DNS server 2
     *
     */
    void WiFi_setDNS_two(ip_addr_t dns_server1, ip_addr_t dns_server2);


    /* Set the hostname used for DHCP requests
     *
     * param name: hostname to set
     *
     */
    void WiFi_setHostname(char* name);

    /*
     * Disconnect from the network
     *
     * return: one value of wl_status_t enum
     */
    int WiFi_disconnect(void);

    void WiFi_end(void);

    /*
     * Get the interface MAC address.
     *
     * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
     */
    uint8_t* WiFi_macAddress(uint8_t* mac);

    /*
     * Get the interface IP address.
     *
     * return: Ip address value
     */
    ip_addr_t WiFi_localIP();

    /*
     * Get the interface subnet mask address.
     *
     * return: subnet mask address value
     */
    ip_addr_t WiFi_subnetMask();

    /*
     * Get the gateway ip address.
     *
     * return: gateway ip address value
     */
   ip_addr_t WiFi_gatewayIP();

    /*
     * Return the current SSID associated with the network
     *
     * return: ssid string
     */
    char* WiFi_SSID();

    /*
      * Return the current BSSID associated with the network.
      * It is the MAC address of the Access Point
      *
      * return: pointer to uint8_t array with length WL_MAC_ADDR_LENGTH
      */
    uint8_t* WiFi_BSSID(uint8_t* bssid);

    /*
      * Return the current RSSI /Received Signal Strength in dBm)
      * associated with the network
      *
      * return: signed value
      */
    int32_t WiFi_RSSI();

    /*
      * Return the Encryption Type associated with the network
      *
      * return: one value of wl_enc_type enum
      */
    uint8_t	WiFi_encryptionType();

    /*
     * Start scan WiFi networks available
     *
     * return: Number of discovered networks
     */
    int8_t WiFi_scanNetworks();

    /*
     * Return the SSID discovered during the network scan.
     *
     * param networkItem: specify from which network item want to get the information
	 *
     * return: ssid string of the specified item on the networks scanned list
     */
    char*	WiFi_SSID_From_Scan(uint8_t networkItem);

    /*
     * Return the encryption type of the networks discovered during the scanNetworks
     *
     * param networkItem: specify from which network item want to get the information
	 *
     * return: encryption type (enum wl_enc_type) of the specified item on the networks scanned list
     */
    uint8_t	WiFi_encryptionType_From_Scan(uint8_t networkItem);

    uint8_t* WiFi_BSSID_From_Scan(uint8_t networkItem, uint8_t* bssid);
    uint8_t WiFi_channel_From_Scan(uint8_t networkItem);

    /*
     * Return the RSSI of the networks discovered during the scanNetworks
     *
     * param networkItem: specify from which network item want to get the information
	 *
     * return: signed value of RSSI of the specified item on the networks scanned list
     */
    int32_t WiFi_RSSI_From_Scan(uint8_t networkItem);

    /*
     * Return Connection status.
     *
     * return: one of the value defined in wl_status_t
     */
    uint8_t WiFi_status();

    /*
     * Return The deauthentication reason code.
     *
     * return: the deauthentication reason code
     */
    uint8_t WiFi_reasonCode();

    /*
     * Resolve the given hostname to an IP address.
     * param aHostname: Name to be resolved
     * param aResult: IPAddress structure to store the returned IP address
     * result: 1 if aIPAddrString was successfully converted to an IP address,
     *          else error code
     */
    int WiFi_hostByName(char* aHostname, ip_addr_t* aResult);

    unsigned long WiFi_getTime();

    void WiFi_lowPowerMode();
    void WiFi_noLowPowerMode();

    int WiFi_ping(char* hostname, uint8_t ttl); //ttl = 128
//    int ping(String &hostname, uint8_t ttl = 128);
    int WiFi_ping_ip(ip_addr_t host, uint8_t ttl); //ttl = 128

    void WiFi_setTimeout(unsigned long timeout);


//extern WiFiClass WiFi;

#endif
