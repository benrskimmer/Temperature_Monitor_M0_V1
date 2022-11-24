/*
 * Application.h
 *
 * Created: 4/20/2022 2:41:18 AM
 *  Author: Ben
 */ 


#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <c_types.h>
#include "app_config.h"
#include "Application_Settings.h"


//////////////////////// PUBLIC TYPE DEFINITIONS ////////////////////////

typedef enum error_states{
	ERROR_UNINITIALIZED_DEVICE = 0,
	ERROR_WIFI_UNAVAILABLE,
	ERROR_WIFI_CREDENTIAL_ISSUE,
	ERROR_NETWORK_ISSUE,
	ERROR_RADIO_MALFUNCTION,
	ERROR_TEMP_SENSOR_MALFUNCTION,
	ERROR_LIGHT_SENSOR_MALFUNCTION,
	ERROR_HARDWARE_MALFUNCTION,
	ERROR_FLASH_MALFUNCTION,
	ERROR_CONFIGURATION_ISSUE,
	ERROR_SERVER_DNS_ISSUE,
	ERROR_SERVER_ISSUE,
	ERROR_DOWNLOADING_OTA,
	ERROR_INSTALLING_OTA,
	ERROR_DOWNLOADING_WIFI_OTA,
	ERROR_INSTALLING_WIFI_OTA,
	ERROR_SYNCING_TIME,
	
	// Add new errors above this line
	ERROR_NUMBER_MAX
} error_states_t;

typedef enum warn_states{
	
	// Add new errors above this line
	WARN_NUMBER_MAX
} warn_states_t;

typedef enum info_states{
	INFO_DOWNLOADING_OTA = 0,
	INFO_OTA_READY,
	INFO_DOWNLOADING_WIFI_OTA,
	INFO_WIFI_OTA_READY,
	INFO_WIFI_OTA_INSTALLING,
	INFO_DRYING_HUMIDITY_SENSOR,
	
	// Add new errors above this line
	INFO_NUMBER_MAX
} info_states_t;

typedef enum app_timers
{
	LT_SENSE_TICK,
	TEMP_SENSE_TICK, // also includes humidity
	SCREEN_UPDATE_TICK, // interval to next screen update (message or scroll cmd depending on state)
	SCREEN_ON_TICK, // amount of time the screen stays on
	SENSOR_DEBUG_TICK, // interval between printing sensor data to CLI
	REMOTE_SYNC_TICK, // interval between updating remote
	NTP_TIME_SYNC_TICK, // interval between syncing system time
	WIFI_CHECK_STAT_TICK, // interval between polling the connection status
	WIFI_RECONNECT_TICK, // interval between retrying to connect to wifi
	
	// Add new timers above this line
	TIMERS_COUNT
} app_timers_t;

typedef enum application_states
{
	INITIALIZATION = 0,
	CONNECTING,
	RUNNING,
	INSTALLING_RADIO_OTA,
} application_states_t;

typedef struct time_config_struct
{
	bool rtc_set; // this is set when we sync with an NTP server, the rest is configured by our BE
	char time_zone[48];
} time_config_t;

typedef struct remote_context_struct
{
	bool wifi_connected;
	uint16_t wifi_connect_count;
	uint16_t wifi_failed_connect_attempts;
	char wifi_ssid[60];
	char wifi_password[40];
	char remote_server[40];
	char ntp_server[40];
	char device_id[40];
	char device_password[17]; // 16 bytes + null termination
	char firmware_version[20];
	char record_version[5];
} remote_context_t;

typedef struct app_state
{
	// state machine
	application_states_t state;
	
	// state information
	uint64_t _errors;
	uint64_t _warnings;
	uint64_t _info;
	
	// settings
	time_config_t time_config;
	bool display_fahrenheit;
	
	// timers
	uint32_t timer_intervals[TIMERS_COUNT];
	int64_t timers[TIMERS_COUNT];
	void (*timer_functions[TIMERS_COUNT]) (void);
	
	// current sensor values
	volatile BOOL motion;
	volatile float temperature;
	volatile float humidity;
	volatile uint16_t lux;
	
	// system state values
	volatile BOOL screen_active;
	uint8_t brightness; // brightness setting for the user LEDs (doesn't include USB interface)
	
	// remote info
	remote_context_t remote;
} app_state_t;

//////////////////////// PUBLIC VARAIBLES ////////////////////////

//////////////////////// PUBLIC FUNCTIONS ////////////////////////

void setError(error_states_t flag);
BOOL getError(error_states_t flag);
void clearError(error_states_t flag);

void setInfo(info_states_t flag);
BOOL getInfo(info_states_t flag);
void clearInfo(info_states_t flag);

// Initializes the application variables - must be called before any other app dependent call
void initApp(void);

// Start the application - initializes all application peripherals
void startApp(void);

// Run the application - runs the system state machine, handles reading data, processing, and communications
void runApp(void);

// create local time string from rtc
void getLocalTimeStr(char* time_str, uint8_t len);

// rtc time functions
void markRtcSet(void);
bool isRtcSet(void);

// set a specified app timer
void setTimer(app_timers_t timer, uint32_t value);

// wifi status logging
void wifiFailedToConnect(void);
void setWifiConnectionState(bool connected);

#endif /* APPLICATION_H_ */