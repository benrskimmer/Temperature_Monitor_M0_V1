/*
 * Remote.c
 *
 * Created: 4/11/2022 2:33:20 AM
 *  Author: Ben
 */ 

#include "Remote.h"
#include "user_board.h"
#include <asf.h>
#include <assert.h>
#include "cJSON.h"
#include "Application.h"
#include "millis.h"

//#define __TM_GMTOFF tm_gmtoff
#include <time.h>
//#include <envz.h>

//////////////////////// PUBLIC DEFINES ////////////////////////


//////////////////////// PRIVATE DEFINES ////////////////////////
#define NTP_PACKET_SIZE 48
#define NTP_RESPONSE_WAIT_TIME 1500
#define BE_RESPONSE_WAIT_TIME 1000
#define WIFI_CONNECTION_TIMEOUT 8000 // 8 seconds
#define BE_CONNECTION_TIMEOUT 5000 // 5 seconds

// Time setting defines
#define SECONDS_SINCE_1970 2208988800UL
#define YEAR_OFFSET	1900
#define MONTH_OFFSET 1

//////////////////////// PUBLIC VARIABLES ////////////////////////
extern struct rtc_module rtc_instance;

//////////////////////// PRIVATE VARIABLES ////////////////////////
bool waiting_on_NTP_response = false;
uint32_t ntp_response_end_time = 0; // time at which we expect a response from the NTP server
bool waiting_on_BE_response = false;
uint32_t BE_response_end_time = 0;
bool received_data_from_BE = false;
bool wifi_connecting = false;
uint32_t wifi_connecting_end_time = 0;
// enum wifi_state_enum
// {
// 	DISCONNECTED,
// 	CONNECTING,
// 	CONNECTED,
// } wifi_state = {DISCONNECTED};

////////////////////// PRIVATE FUNCTION PROTOTYPES //////////////////////
char *build_json_reading_data(app_state_t *app);
bool sendNTPpacket(ip_addr_t address);
void checkNtpResponse(void);
void checkBEResponse(void);
void checkWifiConnecting(void);
void configRadioForUpdate(bool update);


void printMacAddress(uint8_t mac[]) {
	printf("This Device's MAC: ");
	for (int i = 5; i >= 0; i--) {
		if (mac[i] < 16) {
			printf("0");
		}
		printf("%x", mac[i]);
		if (i > 0) {
			printf(":");
		}
	}
	printf("\n");
}

BOOL remoteInit(remote_context_t remote_config)
{
// 	configRadioForUpdate(true);
// 	while(1);
	WiFi_Init();
	uint8_t* mac_address = wifi_drv_getMacAddress();
	printMacAddress(mac_address);
	
	uint8_t num_SSID = wifi_drv_getScanNetworks();
	printf("Number of 2.4GHz Networks: %d\n", num_SSID);
	
	for (int thisNet = 0; thisNet < num_SSID; thisNet++){
		printf("%s\n", wifi_drv_getSSIDNetoworks(thisNet));
	}
	printf("WiFi Firmware Version: %s\n", wifi_drv_getFwVersion());
	printf("WiFi Temp in C: %.5f\n", wifi_drv_getTemperature());
	
	connectToWifi(remote_config);
}

void connectToWifi(remote_context_t remote_config)
{
	if(WiFi_status() != WL_IDLE_STATUS)
	{
		printf("Rebooting the WiFi radio...\n");
		WiFi_disconnect();
		WiFi_end();
		delay(100);
		WiFi_Init();
	}
		
	wifi_drv_debug(true);
	if(strlen(remote_config.wifi_password))
		WiFi_begin_passphrase_async(remote_config.wifi_ssid, remote_config.wifi_password);
	else
		WiFi_begin_open_async(remote_config.wifi_ssid);
	
	wifi_connecting = true;
	wifi_connecting_end_time = millis() + WIFI_CONNECTION_TIMEOUT;
}

// sync device with NTP server
void getTime(void)
{
	if(waiting_on_BE_response) return;
	
	ip_addr_t timeServer;
	IPAddress(&timeServer, 129, 6, 15, 28); // time.nist.gov NTP server
	if(sendNTPpacket(timeServer))
		return;
		
	setTimer(NTP_TIME_SYNC_TICK, TIME_SYNC_RETRY_INTERVAL); // if we weren't able to request the time, retry
}

// send an NTP request to the time server at the given address
bool sendNTPpacket(ip_addr_t address) {

	uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold outgoing packets
	
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);

	// Initialize values needed to form NTP request
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;
	
	unsigned int localPort = 2390;      // local port to listen for UDP packets
	UDP_begin(localPort);
	if(!UDP_beginPacketIP(address, 123)) // NTP requests are to port 123
	{
		printf("Error - failed to send request NTP\n");
		setError(ERROR_SYNCING_TIME);
		return false;
	}
	UDP_write(packetBuffer, NTP_PACKET_SIZE);
	UDP_endPacket();
	printf("Requesting NTP time sync\n");
	
	waiting_on_NTP_response = true;
	ntp_response_end_time = millis() + NTP_RESPONSE_WAIT_TIME;
	return true;
}

// check for a ntp response - called from remote super loop
void checkNtpResponse(void)
{
	if(!waiting_on_NTP_response) return;
	
	if (UDP_parsePacket()) {
		waiting_on_NTP_response = false;
		uint8_t packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming packet
		UDP_read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
		UDP_stop();
		
		unsigned long secsSince1900 = 0;
		secsSince1900 |= (packetBuffer[40] << (8*3));
		secsSince1900 |= (packetBuffer[41] << (8*2));
		secsSince1900 |= (packetBuffer[42] << (8*1));
		secsSince1900 |= (packetBuffer[43] << (8*0));
				
		// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
		time_t epoch_time_now = secsSince1900 - SECONDS_SINCE_1970;
				
		struct tm gmt_time = *gmtime(&epoch_time_now);
				
		// convert 1900 based time epoch to calendar time for RTC
		struct rtc_calendar_time time;
		time.year   = gmt_time.tm_year + YEAR_OFFSET;
		time.month  = gmt_time.tm_mon + MONTH_OFFSET;
		time.day    = gmt_time.tm_mday;
		time.hour   = gmt_time.tm_hour;
		time.minute = gmt_time.tm_min;
		time.second = gmt_time.tm_sec;
				
		/* Set current time. */
		rtc_calendar_set_time(&rtc_instance, &time);
		markRtcSet(); // mark the set flag in the application
				
		printf("Received NTP response!\nEpoch time is: %lu\n", epoch_time_now);
				
		char buf[80];
		strftime(buf, sizeof(buf), "%a %m/%d/%Y %H:%M:%S", &gmt_time);
		printf("GMT Time: %s\n", buf);
				
		//time_t local_time = epoch_time_now - 14400;
		strftime(buf, sizeof(buf), "%a %D %r", localtime(&epoch_time_now));
		printf("Local Time: %s\n\n", buf);
		clearError(ERROR_SYNCING_TIME);
	}
	else if(millis() >= ntp_response_end_time)
	{
		UDP_stop();
		waiting_on_NTP_response = false;
		printf("Error - no NTP response\n");
		setError(ERROR_SYNCING_TIME);
		setTimer(NTP_TIME_SYNC_TICK, TIME_SYNC_RETRY_INTERVAL);
	}
}

// syncs the current app state with the remote server - called by the application
// aka post a "reading"
void remoteSync(app_state_t app)
{	
	if(waiting_on_BE_response || waiting_on_NTP_response || !isRtcSet() || !isWifiConnected()) return;
	
	if(WiFiClient_connectSSLHost(app.remote.remote_server, 443))
	{
		printf("Connected Successfully to %s\n", app.remote.remote_server);
		printf("Wifi status code: %d\n", WiFi_status());
		printf("Wifi rssi: %d\n", WiFi_RSSI());
		printf("Wifi client available: %d\n", WiFiClient_available());
		printf("Wifi client status: %d\n", WiFiClient_status());
		ip_addr_t ip = WiFi_localIP();
		printf("IP Address: ");
		IP_PrintAddress(&ip);
		printf("\n");
		clearError(ERROR_SERVER_DNS_ISSUE);
	}
	else
	{
		printf("Failed to connect to %s\n", app.remote.remote_server);
		printf("Wifi status code: %d\n", WiFi_status());
		printf("Wifi rssi: %d\n", WiFi_RSSI());
		printf("Wifi client available: %d\n", WiFiClient_available());
		printf("Wifi client status: %d\n", WiFiClient_status());
		ip_addr_t ip = WiFi_localIP();
		printf("IP Address: ");
		IP_PrintAddress(&ip);
		printf("\n");
		setError(ERROR_SERVER_DNS_ISSUE);
		return;
	}
	
	char* json_string = build_json_reading_data(&app); // must free this pointer to avoid a memory leak
		
	char* request_msg_str[50] = {0};
	sprintf(request_msg_str, "POST /device/reading/%s HTTP/1.1", app.remote.device_id);
	WiFiClient_writeln(request_msg_str);
	sprintf(request_msg_str, "Host: %s", app.remote.remote_server);
	WiFiClient_writeln(request_msg_str);
	WiFiClient_writeln("Connection: close");
	sprintf(request_msg_str, "Content-Length: %d", strlen(json_string));
	WiFiClient_writeln(request_msg_str);
	WiFiClient_writeln("Content-Type: application/json");
	WiFiClient_writeln("");
	WiFiClient_write(json_string);
	free(json_string);
	
	BE_response_end_time = millis() + BE_RESPONSE_WAIT_TIME;
	waiting_on_BE_response = true;
	received_data_from_BE = false;
	
	printf("\n\n");
}

// check for response from BE
void checkBEResponse(void)
{
	if(!waiting_on_BE_response) return;
	
	if(millis() >= BE_response_end_time)
	{
		waiting_on_BE_response = false;
		setError(ERROR_SERVER_ISSUE);
		printf("\nError - response timeout - disconnecting from server\n");
		WiFiClient_stop();
		return;
	}
	while(WiFiClient_available()) {
		char c = WiFiClient_read_byte();
		printf("%c", c);
		BE_response_end_time = millis() + BE_RESPONSE_WAIT_TIME;
		received_data_from_BE = true;
	}
	if(!WiFiClient_connected())
	{
		if(received_data_from_BE == false)
		{
			setError(ERROR_SERVER_ISSUE);
			printf("\nError - no BE response - disconnecting from server\n");
		}
		WiFiClient_stop();
		waiting_on_BE_response = false;
	}
}

char *build_json_reading_data(app_state_t *app)
{
	// generate UTC BE timestamp (system time is in UTC so offset is "+0000")
	struct rtc_calendar_time rtc_time;
	rtc_calendar_get_time(&rtc_instance, &rtc_time);
	char rtc_string[25] = {0};
	sprintf(rtc_string, "%02d/%02d/%02d %02d:%02d:%02d+0000", rtc_time.month, rtc_time.day, (rtc_time.year%100), rtc_time.hour, rtc_time.minute, rtc_time.second);
	
	char *json_string = NULL;
	cJSON *time_stamp = NULL;
	cJSON *password = NULL;
	cJSON *record_version = NULL;
	cJSON *temperature = NULL;
	cJSON *humidity = NULL;
	cJSON *light_data = NULL;
	cJSON *motion_data = NULL;

	cJSON *reading = cJSON_CreateObject();
	assert(reading != NULL);

	// example timestamp the BE is expecting: "1/17/21 1:12:00+0500" - we're using +0000 as system time is UTC
	time_stamp = cJSON_CreateString(rtc_string);
	assert(time_stamp != NULL);
	
	/* after creation was successful, immediately add it to the monitor,
     * thereby transferring ownership of the pointer to it */
    cJSON_AddItemToObject(reading, "time_stamp", time_stamp);
	
	// password
	password = cJSON_CreateString(app->remote.device_password);
	assert(password != NULL);
	cJSON_AddItemToObject(reading, "password", password);
	
	// record version
	record_version = cJSON_CreateString(app->remote.record_version);
	assert(record_version != NULL);
	cJSON_AddItemToObject(reading, "record_version", record_version);
	
	// temperature
	temperature = cJSON_CreateNumber(app->temperature);
	assert(temperature != NULL);
	cJSON_AddItemToObject(reading, "temperature", temperature);
	
	// humidity
	humidity = cJSON_CreateNumber(app->humidity);
	assert(humidity != NULL);
	cJSON_AddItemToObject(reading, "humidity", humidity);
	
	// light_data
	light_data = cJSON_CreateNumber(app->lux);
	assert(light_data != NULL);
	cJSON_AddItemToObject(reading, "light_data", light_data);
	
	// motion_data
	motion_data = cJSON_CreateNumber(app->motion);
	assert(motion_data != NULL);
	cJSON_AddItemToObject(reading, "motion_data", motion_data);

    json_string = cJSON_Print(reading);
	assert(json_string != NULL);
	cJSON_Delete(reading);
	return json_string;
}

bool isWifiConnected(void)
{
	uint8_t status = WiFi_status();
	return (status == WL_CONNECTED);
}

void checkWifiConnecting(void)
{
	if(!wifi_connecting) return;
	
	bool wifi_connected = isWifiConnected();
	if(!wifi_connected && millis() < wifi_connecting_end_time) // if we haven't waited long enough, don't do anything
		return;
		
	if(!wifi_connected)
		printf("WiFi connection failed! Status code: %d\n", WiFi_status());
		
	wifi_connecting = false;
	setWifiConnectionState(wifi_connected);
}

void runRemote(void)
{
	checkNtpResponse();
	checkBEResponse();
	checkWifiConnecting();
}

void configRadioForUpdate(bool update)
{
	struct port_config output_pin;
	port_get_config_defaults(&output_pin);
	output_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(WIFI_RST_PIN, &output_pin);
	
	if(update)
	{
		port_pin_set_config(WIFI_GPIO0_PIN, &output_pin);
		port_pin_set_output_level(WIFI_GPIO0_PIN, LOW);
		
		port_pin_set_output_level(WIFI_RST_PIN, HIGH);
		delay(100);
		port_pin_set_output_level(WIFI_RST_PIN, LOW);
		delay(100);
	}
	else
	{
		struct port_config input_pin;
		port_get_config_defaults(&input_pin);
		input_pin.direction = PORT_PIN_DIR_INPUT;
		port_pin_set_output_level(WIFI_GPIO0_PIN, LOW);
		port_pin_set_config(WIFI_GPIO0_PIN, &input_pin);
	}
}





// AP EXAMPLE
// 	if (!ECCX08_writeConfiguration(ECCX08_DEFAULT_TLS_CONFIG)) {
//       printf("Writing ECCX08 configuration failed!");
//       while (1);
//     }
// 	else printf("Writing default ECCX08 configuration succeeded");
// 	
// 	if (!ECCX08_lock()) {
// 		printf("Locking ECCX08 configuration failed!");
// 		while (1);
// 	}
// 	else printf("PERMENANTLY LOCKED ECCX08");
	
/*	
	ip_addr_t test_ip;
	wifi_drv_getIpAddress(&test_ip);
	printf("\nOur IP Address: ");
	IP_PrintAddress(&test_ip);
	printf("\n\n");
	
	
	IPAddress(&server_ip, 142, 250, 72, 100);
	*/
// 	ip_addr_t server_ip;
// 	if(WiFi_hostByName(server, &server_ip)){
// 		printf("Resolved IP is: ");
// 		IP_PrintAddress(&server_ip);
// 		printf("\n");
// 	}
// 	else
// 		printf("Couldn't resolve host name");



// 	wifi_drv_pinMode(WIFI_GPIO_R_PIN, 1);
// 	wifi_drv_digitalWrite(WIFI_GPIO_R_PIN, 1);
// 	wifi_drv_pinMode(WIFI_GPIO_G_PIN, 1);
// 	wifi_drv_digitalWrite(WIFI_GPIO_G_PIN, 1);
// 	wifi_drv_pinMode(WIFI_GPIO_B_PIN, 1);
// 	wifi_drv_digitalWrite(WIFI_GPIO_B_PIN, 1);
// 	
// 	printf("WiFi Radio R,G,B are %d,%d,%d,\n",
// 	port_pin_get_input_level(WIFI_GPIO_R_PIN),
// 	port_pin_get_input_level(WIFI_GPIO_G_PIN),
// 	port_pin_get_input_level(WIFI_GPIO_B_PIN));
// 	
// 	printf("WiFi Radio R,G,B are %d,%d,%d,\n",
// 	port_pin_get_input_level(WIFI_GPIO_R_PIN),
// 	port_pin_get_input_level(WIFI_GPIO_G_PIN),
// 	port_pin_get_input_level(WIFI_GPIO_B_PIN));



	/*
	WiFiServer_Init(80);
	WiFi_beginAP_open("Rawstron Industries", 1);
	WiFiServer_begin();
	
	ip_addr_t ip = WiFi_localIP();
	printf("\n\nIP Address: ");
	IP_PrintAddress(&ip);
	printf("\n");
	
	int status = WL_IDLE_STATUS;
	while(1) {
		if (status != WiFi_status()) {
			// it has changed update the variable
			status = WiFi_status();

			if (status == WL_AP_CONNECTED) {
				// a device has connected to the AP
				printf("Device connected to AP\n");
				} else {
				// a device has disconnected from the AP, and we are back in listening mode
				printf("Device disconnected from AP\n");
			}
		}
		
		// listen for incoming clients
		if (WiFiServer_available()) {               // if you get a client,
			printf("new client\n");           // print a message out the serial port
			char currentLine[256] = "";                // make a String to hold incoming data from the client
			uint16_t lineIndex = 0;
			while (WiFiClient_connected()) {            // loop while the client's connected
				if (WiFiClient_available()) {             // if there's bytes to read from the client,
					char c = WiFiClient_read_byte();             // read a byte, then
					printf("%c", c);                    // print it out the serial monitor
					if (c == '\n') {                    // if the byte is a newline character

						// if the current line is blank, you got two newline characters in a row.
						// that's the end of the client HTTP request, so send a response:
						if (strlen(currentLine) == 0) {
							// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
							// and a content-type so the client knows what's coming, then a blank line:
							WiFiClient_writeln("HTTP/1.1 200 OK");
							WiFiClient_writeln("Content-type:text/html");
							WiFiClient_writeln("");

							// the content of the HTTP response follows the header:
							WiFiClient_write( "<!DOCTYPE html>" );
							WiFiClient_write( "<html lang=\"en\">" );
							WiFiClient_write( "<head><meta charset=\"UTF-8\"><meta name=\"HandheldFriendly\" content=\"True\">" );
							WiFiClient_write( "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.5, user-scalable=yes\"></head>" );
							WiFiClient_write( "<body>" );
							WiFiClient_write("Click <a href=\"/H\">here</a> turn the LED on<br>");
							WiFiClient_write("Click <a href=\"/L\">here</a> turn the LED off<br>");
							WiFiClient_write( "</body></html>" );

							// The HTTP response ends with another blank line:
							WiFiClient_writeln("");
							// break out of the while loop:
							break;
						}
						else {      // if you got a newline, then clear currentLine:
							currentLine[0] = 0;
							lineIndex = 0;
						}
					}
					else if (c != '\r') {    // if you got anything else but a carriage return character,
						currentLine[lineIndex++] = c;      // add it to the end of the currentLine
					}

					// Check to see if the client request was "GET /H" or "GET /L":
					if (EndsWith(currentLine, "GET /H")) {
						//port_pin_set_output_level(LED_PIN, HIGH);               // GET /H turns the LED on
						
						rgb_t_t LED_temp = {100, 0, 100};
						apa102_setleds(&LED_temp, 1);
						
						msgs[0] = (char *)"NIIICE!!!";
						oledFill(&screen, 0, 1);
						oledSetTextWrap(&screen, 1);
						oledWriteString(&screen, 0,25,2,msgs[rc], FONT_NORMAL, 0, 1);
					}
					if (EndsWith(currentLine, "GET /L")) {
						//port_pin_set_output_level(LED_PIN, LOW);                // GET /L turns the LED off
						
						rgb_t_t LED_temp = {1, 1, 1};
						apa102_setleds(&LED_temp, 1);
						
						msgs[0] = (char *)"Hey!! Turn that sh*t back on!";
						oledFill(&screen, 0, 1);
						oledSetTextWrap(&screen, 1);
						oledWriteString(&screen, 0,0,0,msgs[rc], FONT_NORMAL, 0, 1);
					}
				}
			}
			// close the connection:
			WiFiClient_stop();
			printf("client disconnected\n");
		}
	}
	*/
	
	
	// 	uint8_t ch_from_USB = 0;
	// 	uint8_t ch_from_SPI = 0;
	
	// 	check for the WiFi module:
	// 		if (WiFi.status() == WL_NO_MODULE) {
	// 			printf("Communication with WiFi module failed!\n");
	// 			// don't continue
	// 			while (true);
	// 		}
	//
	// 		char fv[20] = WiFi.firmwareVersion();
	// 		printf("%s", fv);
	// 		if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
	// 			printf("Please upgrade the firmware\n");
	// 		}
	//
	// 		// print your MAC address:
	// 		uint8_t mac[6];
	// 		WiFi.macAddress(mac);
	// 		printf("MAC: ");
	// 		printMacAddress(mac);
	//
	// 		printf("Scanning available networks...");
	// 		listNetworks();