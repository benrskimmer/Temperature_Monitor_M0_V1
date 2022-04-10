/*
 * CFile1.c
 *
 * Created: 7/27/2021 7:20:58 PM
 *  Author: Ben
 */ 

#include <stdio.h>
#include <string.h>
#include <stdint.h>

//#include "Arduino.h"
#include "spi_drv.h"
#include "wifi_drv.h"
#include "wifi_spi.h"
#include "wl_types.h"
#include "debug.h"

#define _DEBUG_



char _networkSsid[WL_NETWORKS_LIST_MAXNUM][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};

// firmware version string in the format a.b.c
char 	fwVersion[WL_FW_VER_LENGTH] = {0};

// settings of current selected network
char 	_ssid[WL_SSID_MAX_LENGTH] = {0};
uint8_t 	_bssid[WL_MAC_ADDR_LENGTH] = {0};
uint8_t 	_mac[WL_MAC_ADDR_LENGTH] = {0};
uint8_t  _localIp[WL_IPV4_LENGTH] = {0};
uint8_t  _subnetMask[WL_IPV4_LENGTH] = {0};
uint8_t  _gatewayIp[WL_IPV4_LENGTH] = {0};



// local functions

/*
	* Get network Data information
	*/
// void wifi_drv_getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip);
// 
uint8_t wifi_drv_reqHostByName(const char* aHostname);

int wifi_drv_getHostByName_private(ip_addr_t* aResult);
	
	
	

// Array of data to cache the information related to the networks discovered
//char _networkSsid[][WL_SSID_MAX_LENGTH] = {{"1"},{"2"},{"3"},{"4"},{"5"}};

// Cached values of retrieved data
// char 	wifi_drv__ssid[] = {0};
// uint8_t	wifi_drv__bssid[] = {0};
// uint8_t wifi_drv__mac[] = {0};
// uint8_t wifi_drv__localIp[] = {0};
// uint8_t wifi_drv__subnetMask[] = {0};
// uint8_t wifi_drv__gatewayIp[] = {0};
// // Firmware version
// char    wifi_drv_fwVersion[] = {0};


// Private Methods

void wifi_drv_getNetworkData(uint8_t *ip, uint8_t *mask, uint8_t *gwip) //UPDATE LEAVING OFF HERE
{
	tParam params[PARAM_NUMS_3] = { {0, (char*)ip}, {0, (char*)mask}, {0, (char*)gwip}};

	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_IPADDR_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, sizeof(_dummy), LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	wifi_spi_waitResponseParams(GET_IPADDR_CMD, PARAM_NUMS_3, params);

	wifi_spi_spiSlaveDeselect();
}

void wifi_drv_getRemoteData(uint8_t sock, uint8_t *ip, uint8_t *port)
{
	tParam params[PARAM_NUMS_2] = { {0, (char*)ip}, {0, (char*)port} };

	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_REMOTE_DATA_CMD, PARAM_NUMS_1);
	wifi_spi_sendParamLen(&sock, sizeof(sock), LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	wifi_spi_waitResponseParams(GET_REMOTE_DATA_CMD, PARAM_NUMS_2, params);

	wifi_spi_spiSlaveDeselect();
}


// Public Methods


void wifi_drv_wifiDriverInit()
{
	wifi_spi_begin();
}

void wifi_drv_wifiDriverDeinit()
{
	wifi_spi_end();
}

int8_t wifi_drv_wifiSetNetwork(const char* ssid, uint8_t ssid_len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_NET_CMD, PARAM_NUMS_1);
	wifi_spi_sendParamLen((uint8_t*)ssid, ssid_len, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 5 + ssid_len;
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
	if (!wifi_spi_waitResponseCmd(SET_NET_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		//WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();

	return(_data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
}

int8_t wifi_drv_wifiSetPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)passphrase, len, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 6 + ssid_len + len;
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
	if (!wifi_spi_waitResponseCmd(SET_PASSPHRASE_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		//WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}


int8_t wifi_drv_wifiSetKey(const char* ssid, uint8_t ssid_len, uint8_t key_idx, const void *key, const uint8_t len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_KEY_CMD, PARAM_NUMS_3);
	wifi_spi_sendParamLen((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
	wifi_spi_sendParamLen(&key_idx, KEY_IDX_LEN, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)key, len, LAST_PARAM);
	
	// pad to multiple of 4
	int commandSize = 8 + ssid_len + len;
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
	if (!wifi_spi_waitResponseCmd(SET_KEY_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		//WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

void wifi_drv_config(uint8_t validParams, uint32_t local_ip, uint32_t gateway, uint32_t subnet)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_IP_CONFIG_CMD, PARAM_NUMS_4);
	wifi_spi_sendParamLen((uint8_t*)&validParams, 1, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&local_ip, 4, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&gateway, 4, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&subnet, 4, LAST_PARAM);

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
	if (!wifi_spi_waitResponseCmd(SET_IP_CONFIG_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		//WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
}

void wifi_drv_setDNS(uint8_t validParams, uint32_t dns_server1, uint32_t dns_server2)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_DNS_CONFIG_CMD, PARAM_NUMS_3);
	wifi_spi_sendParamLen((uint8_t*)&validParams, 1, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&dns_server1, 4, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&dns_server2, 4, LAST_PARAM);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = 0;
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(SET_DNS_CONFIG_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
}

void wifi_drv_setHostname(const char* hostname)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_HOSTNAME_CMD, PARAM_NUMS_1);
	wifi_spi_sendParamLen((uint8_t*)hostname, strlen(hostname), LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 5 + strlen(hostname);
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
	if (!wifi_spi_waitResponseCmd(SET_HOSTNAME_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
}

int8_t wifi_drv_disconnect()
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(DISCONNECT_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, 1, LAST_PARAM);

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
	int8_t result = wifi_spi_waitResponseCmd(DISCONNECT_CMD, PARAM_NUMS_1, &_data, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return result;
}

uint8_t wifi_drv_getReasonCode()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_REASON_CODE_CMD, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = 1;
	uint8_t _dataLen = 0;
	wifi_spi_waitResponseCmd(GET_REASON_CODE_CMD, PARAM_NUMS_1, &_data, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return _data;
}

uint8_t wifi_drv_getConnectionStatus()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = -1;
	uint8_t _dataLen = 0;
	wifi_spi_waitResponseCmd(GET_CONN_STATUS_CMD, PARAM_NUMS_1, &_data, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return _data;
}

uint8_t* wifi_drv_getMacAddress()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_MACADDR_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, 1, LAST_PARAM);
	
	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	wifi_spi_waitResponseCmd(GET_MACADDR_CMD, PARAM_NUMS_1, _mac, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return _mac;
}

void wifi_drv_getIpAddress(ip_addr_t* ip)
{
	wifi_drv_getNetworkData(_localIp, _subnetMask, _gatewayIp);
	IPAddress_From_UVAR8(ip, &_localIp);
}

void wifi_drv_getSubnetMask(ip_addr_t* mask)
{
	wifi_drv_getNetworkData(_localIp, _subnetMask, _gatewayIp);
	IPAddress_From_UVAR8(mask, &_subnetMask);
}

void wifi_drv_getGatewayIP(ip_addr_t* ip)
{
	wifi_drv_getNetworkData(_localIp, _subnetMask, _gatewayIp);
	IPAddress_From_UVAR8(ip, _gatewayIp);
}

const char* wifi_drv_getCurrentSSID()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	memset(_ssid, 0x00, sizeof(_ssid));

	// Wait for reply
	uint8_t _dataLen = 0;
	wifi_spi_waitResponseCmd(GET_CURR_SSID_CMD, PARAM_NUMS_1, (uint8_t*)_ssid, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return _ssid;
}

uint8_t* wifi_drv_getCurrentBSSID()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	wifi_spi_waitResponseCmd(GET_CURR_BSSID_CMD, PARAM_NUMS_1, _bssid, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return _bssid;
}

int32_t wifi_drv_getCurrentRSSI()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	int32_t rssi = 0;
	wifi_spi_waitResponseCmd(GET_CURR_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&rssi, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	return rssi;
}

uint8_t wifi_drv_getCurrentEncryptionType()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1);

	uint8_t _dummy = DUMMY_DATA;
	wifi_spi_sendParamLen(&_dummy, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	uint8_t encType = 0;
	wifi_spi_waitResponseCmd(GET_CURR_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)&encType, &dataLen);

	wifi_spi_spiSlaveDeselect();

	return encType;
}

int8_t wifi_drv_startScanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(START_SCAN_NETWORKS, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = 0;
	uint8_t _dataLen = 0;

	if (!wifi_spi_waitResponseCmd(START_SCAN_NETWORKS, PARAM_NUMS_1, &_data, &_dataLen))
	{
		//WARN("error waitResponse");
		_data = WL_FAILURE;
	}

	wifi_spi_spiSlaveDeselect();

	return ((int8_t)_data == WL_FAILURE)? _data : (int8_t)WL_SUCCESS;
}


uint8_t wifi_drv_getScanNetworks()
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(SCAN_NETWORKS, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t ssidListNum = 0;
	wifi_spi_waitResponse(SCAN_NETWORKS, &ssidListNum, (uint8_t**)_networkSsid, WL_NETWORKS_LIST_MAXNUM);

	wifi_spi_spiSlaveDeselect();

	return ssidListNum;
}

const char* wifi_drv_getSSIDNetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
	return (char*)NULL;

	return _networkSsid[networkItem];
}

uint8_t wifi_drv_getEncTypeNetowrks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
	return ENC_TYPE_UNKNOWN;

	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_IDX_ENCT_CMD, PARAM_NUMS_1);

	wifi_spi_sendParamLen(&networkItem, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	uint8_t encType = 0;
	wifi_spi_waitResponseCmd(GET_IDX_ENCT_CMD, PARAM_NUMS_1, (uint8_t*)&encType, &dataLen);

	wifi_spi_spiSlaveDeselect();

	return encType;
}

uint8_t* wifi_drv_getBSSIDNetowrks(uint8_t networkItem, uint8_t* bssid)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
	return NULL;

	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_IDX_BSSID, PARAM_NUMS_1);

	wifi_spi_sendParamLen(&networkItem, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	wifi_spi_waitResponseCmd(GET_IDX_BSSID, PARAM_NUMS_1, (uint8_t*)bssid, &dataLen);

	wifi_spi_spiSlaveDeselect();

	return bssid;
}

uint8_t wifi_drv_getChannelNetowrks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
	return 0;

	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_IDX_CHANNEL_CMD, PARAM_NUMS_1);

	wifi_spi_sendParamLen(&networkItem, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	uint8_t channel = 0;
	wifi_spi_waitResponseCmd(GET_IDX_CHANNEL_CMD, PARAM_NUMS_1, (uint8_t*)&channel, &dataLen);

	wifi_spi_spiSlaveDeselect();

	return channel;
}

int32_t wifi_drv_getRSSINetoworks(uint8_t networkItem)
{
	if (networkItem >= WL_NETWORKS_LIST_MAXNUM)
	return 0;
	int32_t	networkRssi = 0;

	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(GET_IDX_RSSI_CMD, PARAM_NUMS_1);

	wifi_spi_sendParamLen(&networkItem, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	wifi_spi_waitResponseCmd(GET_IDX_RSSI_CMD, PARAM_NUMS_1, (uint8_t*)&networkRssi, &dataLen);

	wifi_spi_spiSlaveDeselect();

	return networkRssi;
}

uint8_t wifi_drv_reqHostByName(const char* aHostname)
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(REQ_HOST_BY_NAME_CMD, PARAM_NUMS_1);
	wifi_spi_sendParamLen((uint8_t*)aHostname, strlen(aHostname), LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 5 + strlen(aHostname);
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
	uint8_t result = wifi_spi_waitResponseCmd(REQ_HOST_BY_NAME_CMD, PARAM_NUMS_1, &_data, &_dataLen);

	wifi_spi_spiSlaveDeselect();

	if (result) {
		result = (_data == 1);
	}

	return result;
}

int wifi_drv_getHostByName_private(ip_addr_t* aResult)
{
	uint8_t  _ipAddr[WL_IPV4_LENGTH];
	ip_addr_t dummy;
	IPAddress(&dummy,0xFF,0xFF,0xFF,0xFF);
	int result = 0;

	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(GET_HOST_BY_NAME_CMD, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(GET_HOST_BY_NAME_CMD, PARAM_NUMS_1, _ipAddr, &_dataLen))
	{
		WARN("error waitResponse");
	}else{
//		ip_2_ip4(aResult)->addr = _ipAddr;
		IPAddress_From_UVAR8(aResult, _ipAddr);
		result = !ip_addr_cmp(aResult, &dummy);
	}
	wifi_spi_spiSlaveDeselect();
	return result;
}

int wifi_drv_getHostByName(const char* aHostname, ip_addr_t* aResult)
{
	if (wifi_drv_reqHostByName(aHostname))
	{
		return wifi_drv_getHostByName_private(aResult);
	}else{
	return 0;
	}
}

const char*  wifi_drv_getFwVersion()
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(GET_FW_VERSION_CMD, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(GET_FW_VERSION_CMD, PARAM_NUMS_1, (uint8_t*)fwVersion, &_dataLen))
	{
		WARN("error waitResponse");
	}
	wifi_spi_spiSlaveDeselect();
	return fwVersion;
}

uint32_t wifi_drv_getTime()
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(GET_TIME_CMD, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	uint32_t _data = 0;
	if (!wifi_spi_waitResponseCmd(GET_TIME_CMD, PARAM_NUMS_1, (uint8_t*)&_data, &_dataLen))
	{
		WARN("error waitResponse");
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

void wifi_drv_setPowerMode(uint8_t mode)
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(SET_POWER_MODE_CMD, PARAM_NUMS_1);

	wifi_spi_sendParamLen(&mode, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	uint8_t data = 0;
	wifi_spi_waitResponseCmd(SET_POWER_MODE_CMD, PARAM_NUMS_1, &data, &dataLen);

	wifi_spi_spiSlaveDeselect();
}

int8_t wifi_drv_wifiSetApNetwork(const char* ssid, uint8_t ssid_len, uint8_t channel)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_AP_NET_CMD, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
	wifi_spi_sendParamLen(&channel, 1, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 3 + ssid_len;
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
	if (!wifi_spi_waitResponseCmd(SET_AP_NET_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();

	return(_data == WIFI_SPI_ACK) ? WL_SUCCESS : WL_FAILURE;
}

int8_t wifi_drv_wifiSetApPassphrase(const char* ssid, uint8_t ssid_len, const char *passphrase, const uint8_t len, uint8_t channel)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_AP_PASSPHRASE_CMD, PARAM_NUMS_3);
	wifi_spi_sendParamLen((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)passphrase, len, NO_LAST_PARAM);
	wifi_spi_sendParamLen(&channel, 1, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 4 + ssid_len + len;
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
	if (!wifi_spi_waitResponseCmd(SET_AP_PASSPHRASE_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

int8_t wifi_drv_wifiSetEnterprise(uint8_t eapType, const char* ssid, uint8_t ssid_len, const char *username, const uint8_t username_len, const char *password, const uint8_t password_len, const char *identity, const uint8_t identity_len, const char* ca_cert, uint16_t ca_cert_len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_ENT_CMD, PARAM_NUMS_6);
	wifi_spi_sendBuffer(&eapType, sizeof(eapType), NO_LAST_PARAM);
	wifi_spi_sendBuffer((uint8_t*)ssid, ssid_len, NO_LAST_PARAM);
	wifi_spi_sendBuffer((uint8_t*)username, username_len, NO_LAST_PARAM);
	wifi_spi_sendBuffer((uint8_t*)password, password_len, NO_LAST_PARAM);
	wifi_spi_sendBuffer((uint8_t*)identity, identity_len, NO_LAST_PARAM);
	wifi_spi_sendBuffer((uint8_t*)ca_cert, ca_cert_len, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 15 + sizeof(eapType) + ssid_len + username_len + password_len + identity_len + ca_cert_len;
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
	if (!wifi_spi_waitResponseCmd(SET_ENT_CMD, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

int16_t wifi_drv_ping(uint32_t ipAddress, uint8_t ttl)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(PING_CMD, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)&ipAddress, sizeof(ipAddress), NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&ttl, sizeof(ttl), LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint16_t _data;
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(PING_CMD, PARAM_NUMS_1, (uint8_t*)&_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_PING_ERROR;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

void wifi_drv_debug(uint8_t on)
{
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(SET_DEBUG_CMD, PARAM_NUMS_1);

	wifi_spi_sendParamLen(&on, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t dataLen = 0;
	uint8_t data = 0;
	wifi_spi_waitResponseCmd(SET_DEBUG_CMD, PARAM_NUMS_1, &data, &dataLen);

	wifi_spi_spiSlaveDeselect();
}

float wifi_drv_getTemperature()
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(GET_TEMPERATURE_CMD, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _dataLen = 0;
	float _data = 0;
	if (!wifi_spi_waitResponseCmd(GET_TEMPERATURE_CMD, PARAM_NUMS_1, (uint8_t*)&_data, &_dataLen))
	{
		WARN("error waitResponse");
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

void wifi_drv_pinMode(uint8_t pin, uint8_t mode)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_PIN_MODE, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)&pin, 1, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&mode, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = 0;
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(SET_PIN_MODE, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
}

void wifi_drv_digitalWrite(uint8_t pin, uint8_t value)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_DIGITAL_WRITE, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)&pin, 1, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&value, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = 0;
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(SET_DIGITAL_WRITE, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
}

void wifi_drv_analogWrite(uint8_t pin, uint8_t value)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(SET_ANALOG_WRITE, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)&pin, 1, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&value, 1, LAST_PARAM);

	// pad to multiple of 4
	wifi_spi_readChar();

	wifi_spi_spiSlaveDeselect();
	//Wait the reply elaboration
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	// Wait for reply
	uint8_t _data = 0;
	uint8_t _dataLen = 0;
	if (!wifi_spi_waitResponseCmd(SET_ANALOG_WRITE, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
}

int8_t wifi_drv_downloadFile(const char* url, uint8_t url_len, const char *filename, uint8_t filename_len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(DOWNLOAD_FILE, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)url, url_len, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)filename, filename_len, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 6 + url_len + filename_len;
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
	if (!wifi_spi_waitResponseCmd(DOWNLOAD_FILE, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

int8_t wifi_drv_downloadOTA(const char* url, uint8_t url_len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	wifi_spi_sendCmd(DOWNLOAD_OTA, PARAM_NUMS_1);
	wifi_spi_sendParamLen((uint8_t*)url, url_len, LAST_PARAM);

	// pad to multiple of 4
	int commandSize = 5 + url_len;
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
	if (!wifi_spi_waitResponseCmd(DOWNLOAD_OTA, PARAM_NUMS_1, &_data, &_dataLen))
	{
		WARN("error waitResponse");
		_data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return _data;
}

int8_t wifi_drv_renameFile(const char * old_file_name, uint8_t const old_file_name_len, const char * new_file_name, uint8_t const new_file_name_len)
{
	WAIT_FOR_SLAVE_SELECT();
	/* Send Command */
	wifi_spi_sendCmd(RENAME_FILE, PARAM_NUMS_2);
	wifi_spi_sendParamLen((uint8_t*)old_file_name, old_file_name_len, NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)new_file_name, new_file_name_len, LAST_PARAM);

	/* pad to multiple of 4 */
	int commandSize = 6 + old_file_name_len + new_file_name_len;
	while (commandSize % 4) {
		wifi_spi_readChar();
		commandSize++;
	}

	wifi_spi_spiSlaveDeselect();
	/* Wait the reply elaboration */
	wifi_spi_waitForSlaveReady();
	wifi_spi_spiSlaveSelect();

	/* Wait for reply */
	uint8_t data = 0;
	uint8_t dataLen = 0;
	if (!wifi_spi_waitResponseCmd(RENAME_FILE, PARAM_NUMS_1, &data, &dataLen))
	{
		WARN("error waitResponse");
		data = WL_FAILURE;
	}
	wifi_spi_spiSlaveDeselect();
	return data;
}

int8_t wifi_drv_fileOperation(uint8_t operation, const char *filename, uint8_t filename_len, uint32_t offset, uint8_t* buffer, uint32_t len)
{
	WAIT_FOR_SLAVE_SELECT();
	// Send Command
	uint8_t numParams = PARAM_NUMS_3;
	if (operation == WRITE_FILE) {
		numParams = PARAM_NUMS_4;
	}

	wifi_spi_sendCmd(operation, numParams);
	wifi_spi_sendParamLen((uint8_t*)&offset, sizeof(offset), NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)&len, sizeof(len), NO_LAST_PARAM);
	wifi_spi_sendParamLen((uint8_t*)filename, filename_len, (operation == WRITE_FILE) ? NO_LAST_PARAM : LAST_PARAM);
	if (operation == WRITE_FILE) {
		wifi_spi_sendParamNoLen((uint8_t*)buffer, len, LAST_PARAM);
	}

	// pad to multiple of 4
	int commandSize = 4 + numParams + sizeof(offset) + sizeof(len) + filename_len;
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
	wifi_spi_waitResponseCmd(operation, PARAM_NUMS_1, (operation == WRITE_FILE) ? &_data : buffer, &_dataLen);

	wifi_spi_spiSlaveDeselect();
	return _dataLen;
}

void wifi_drv_applyOTA() {
	WAIT_FOR_SLAVE_SELECT();

	// Send Command
	wifi_spi_sendCmd(APPLY_OTA_COMMAND, PARAM_NUMS_0);

	wifi_spi_spiSlaveDeselect();

	// don't wait for return; OTA operation should be fire and forget :)
}


