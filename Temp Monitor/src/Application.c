/*
 * Application.c
 *
 * Created: 4/20/2022 2:40:54 AM
 *  Author: Ben
 */ 

#include "Application.h"

#include <stdint-gcc.h>
#include <assert.h>

//#define __TM_GMT0FF tm_gmtoff
#define __XSI_VISIBLE TRUE
#include <time.h>

// include hardware drivers
#include "Hardware.h"

#include "Remote.h"
#include "Sensors.h"
#include "LED_Utils.h"
#include "millis.h"
#include "screen_msg_buffer.h"


///////////////////////////// PRIVATE DEFINES /////////////////////////////
#define RELOAD_TIMER(x) assert(app.timer_intervals[x]); app.timers[x] = app.timer_intervals[x]
#define STOP_TIMER(x) app.timers[x] = -1
#define SET_RELOAD_INTERVAL(x) app.timer_intervals[x]
#define CLEAR_RELOAD_INTERVAL(x) app.timer_intervals[x] = 0
#define SET_TIMER_INTERVAL(x,y) app.timers[x] = y
#define TIMER_CALLBACK(x) app.timer_functions[x]


////////////////////// PRIVATE FUNCTION DEFINITIONS //////////////////////

void loadTimers(void);
void updateTimers(void);
void screenSaver(void);
void pollLightSensor(void);
void pollHumidityTempSensor(void);
void printSensorDebug(void);
void remoteSyncCallback(void);
void wifiCheckConnectionStatus(void);
void wifiReconnectCallback(void);
void pollMotionSensor(void);



//////////////////////// PRIVATE TYPE DEFINITIONS ////////////////////////

//////////////////////// PUBLIC VARIABLES ////////////////////////
extern struct rtc_module rtc_instance;

//////////////////////// PRIVATE VARIABLES ////////////////////////
app_state_t app = {.state = INITIALIZATION};

//////////////////////// PUBLIC FUNCTIONS ////////////////////////

void setError(error_states_t flag){ app._errors |= (uint64_t)(1 << flag); }
BOOL getError(error_states_t flag){ return app._errors & (uint64_t)(1 << flag); }
void clearError(error_states_t flag){ app._errors &= ~((uint64_t)(1 << flag)); }

void setInfo(info_states_t flag){ app._info |= (uint64_t)(1 << flag); }
BOOL getInfo(info_states_t flag){ return app._info & (uint64_t)(1 << flag); }
void clearInfo(info_states_t flag){ app._info &= ~((uint64_t)(1 << flag)); }

// Initializes the application variables - must be called before any other app dependent call
void initApp(void)
{
	if(app.state != INITIALIZATION) return;
	
	// setup wifi configuration
	app.remote.wifi_connected = false;
	app.remote.wifi_connect_count = 0;
	app.remote.wifi_failed_connect_attempts = 0;
	
#ifdef HARD_CODE_WIFI_SSID
	assert(strlen(HARD_CODE_WIFI_SSID) < sizeof(app.remote.wifi_ssid));
	strcpy(app.remote.wifi_ssid, HARD_CODE_WIFI_SSID);
#else
	strcpy(app.remote.wifi_ssid, ""); // default to empty string to indicate setup incomplete
#endif /* #ifdef HARD_CODE_WIFI_SSID */

#ifdef HARD_CODE_WIFI_PASSWORD
	assert(strlen(HARD_CODE_WIFI_PASSWORD) < sizeof(app.remote.wifi_password));
	strcpy(app.remote.wifi_password, HARD_CODE_WIFI_PASSWORD);
#else
	strcpy(app.remote.wifi_password, ""); // default to empty string to indicate open network
#endif /* #ifdef HARD_CODE_WIFI_PASSWORD */
	
	// set the back end remote server
#if defined HARD_CODE_BE_TEST_URL
	assert(strlen(HARD_CODE_BE_TEST_URL) < sizeof(app.remote.remote_server));
	strcpy(app.remote.remote_server, HARD_CODE_BE_TEST_URL);
#elif defined HARD_CODE_BE_POSTMAN_URL
	assert(strlen(HARD_CODE_BE_POSTMAN_URL) < sizeof(app.remote.remote_server));
	strcpy(app.remote.remote_server, HARD_CODE_BE_POSTMAN_URL);
#else
	assert(strlen(PROD_BE_SERVER) < sizeof(app.remote.remote_server));
	strcpy(app.remote.remote_server, PROD_BE_SERVER);
#endif /* #if defined HARD_CODE_BE_TEST_URL */

	// set NTP server
	assert(strlen(NTP_TIME_SERVER) < sizeof(app.remote.ntp_server));
	strcpy(app.remote.ntp_server, NTP_TIME_SERVER);
	
#ifdef HARD_CODE_DEVICE_ID
	assert(strlen(HARD_CODE_DEVICE_ID) < sizeof(app.remote.device_id));
	strcpy(app.remote.device_id, HARD_CODE_DEVICE_ID);
#endif /* #ifdef HARD_CODE_DEVICE_ID */

#ifdef HARD_CODE_DEVICE_PASSWORD
	assert(strlen(HARD_CODE_DEVICE_PASSWORD) < sizeof(app.remote.device_password));
	strcpy(app.remote.device_password, HARD_CODE_DEVICE_PASSWORD);
#endif /* #ifdef HARD_CODE_DEVICE_PASSWORD */

	assert(strlen(FIRMWARE_VERSION) < sizeof(app.remote.firmware_version));
	strcpy(app.remote.firmware_version, FIRMWARE_VERSION);

	assert(strlen(RECORD_VERSION) < sizeof(app.remote.record_version));
	strcpy(app.remote.record_version, RECORD_VERSION);
	
	app.time_config.rtc_set = false;
#ifdef DEFAULT_EST_TIMEZONE
	memcpy(app.time_config.time_zone, "EST+5EDT,M3.2.0,M11.1.0", strlen("EST+5EDT,M3.2.0,M11.1.0"));
#else
	app.time_config.time_zone[0] = 0; // ensure string is null
#endif /* #ifdef DEFAULT_EST_TIMEZONE */

	app.brightness = 40;
	app.screen_active = true;
	app.display_fahrenheit = true;
	
	// default to invalid value so averaging can tell it's uninitialized
	app.temperature = 0xFFFFFFFF;
	app.humidity = 0xFFFFFFFF;
	app.lux = 0xFFFF; 
}

// Start the normal application - initializes all app peripherals
void startApp(void)
{
	configureSensors();
	
	setUserLEDColor(WHITE, app.brightness);
	
	app.state = CONNECTING;
}

// Run the application - runs the system state machine, handles reading data, processing, and communications
void runApp(void)
{
	switch(app.state)
	{
		case CONNECTING:
			remoteInit(app.remote);
			// on bootup, only start timers after we've tried to connect to the BE
			loadTimers();
			app.state = RUNNING;
		break;
	}
	
	// update user interface
	updateTimers();
}


//////////////////////// PRIVATE FUNCTIONS ////////////////////////

void loadTimers(void)
{
	// Set intervals
	SET_RELOAD_INTERVAL(LT_SENSE_TICK) = DEFAULT_FAST_LT_SENSE_POLL_INTERVAL;
	SET_RELOAD_INTERVAL(TEMP_SENSE_TICK) = DEFAULT_HUMIDITY_TEMP_SENSE_POLL_INTERVAL;
	SET_RELOAD_INTERVAL(SCREEN_ON_TICK) = DEFAULT_SCREEN_ON_DURATION;
	SET_RELOAD_INTERVAL(SENSOR_DEBUG_TICK) = DEFAULT_SENSOR_DEBUG_INTERVAL;
	SET_RELOAD_INTERVAL(NTP_TIME_SYNC_TICK) = DEFAULT_TIME_SYNC_INTERVAL;
	SET_RELOAD_INTERVAL(REMOTE_SYNC_TICK) = DEFAULT_REMOTE_SYNC_INTERVAL;
	SET_RELOAD_INTERVAL(WIFI_RECONNECT_TICK) = WIFI_RECONNECT_INTERVAL;
	SET_RELOAD_INTERVAL(WIFI_CHECK_STAT_TICK) = WIFI_CHECK_STAT_INTERVAL;
	
	// Default timers to stopped state (-1)
	for(uint8_t i = 0; i < TIMERS_COUNT; i++) 
		app.timers[i] = -1;
	
	// Load counters
	RELOAD_TIMER(LT_SENSE_TICK);
	RELOAD_TIMER(TEMP_SENSE_TICK);
	RELOAD_TIMER(SCREEN_ON_TICK);
	RELOAD_TIMER(SENSOR_DEBUG_TICK);
	RELOAD_TIMER(REMOTE_SYNC_TICK);
	RELOAD_TIMER(NTP_TIME_SYNC_TICK);
	RELOAD_TIMER(WIFI_CHECK_STAT_TICK);
	
	// Set callback functions
	// first default all functions to NULL, some code relies on this to determine if they're valid functions
	memset(app.timer_functions, NULL, sizeof(app.timer_functions));
	
	// List out all callback functions beneath this line
	TIMER_CALLBACK(LT_SENSE_TICK) = pollLightSensor;
	TIMER_CALLBACK(TEMP_SENSE_TICK) = pollHumidityTempSensor;
	TIMER_CALLBACK(SENSOR_DEBUG_TICK) = printSensorDebug;
	TIMER_CALLBACK(SCREEN_ON_TICK) = screenSaver;
	TIMER_CALLBACK(REMOTE_SYNC_TICK) = remoteSyncCallback;
	TIMER_CALLBACK(NTP_TIME_SYNC_TICK) = getTime;
	TIMER_CALLBACK(WIFI_RECONNECT_TICK) = wifiReconnectCallback;
	TIMER_CALLBACK(WIFI_CHECK_STAT_TICK) = wifiCheckConnectionStatus;
	print_message_repeat("Hi", -1, LOWEST_MSG_PRIORITY);
	print_message_repeat("Another random message", -1, LOWEST_MSG_PRIORITY);
}

void updateTimers(void)
{
	uint16_t time_elapsed = elapsedMS();
	if(!time_elapsed) return;
	
	// handle callbacks
	for(int i = 0; i < TIMERS_COUNT; i++)
	{
		if(app.timers[i] > time_elapsed)
			app.timers[i] -= time_elapsed;
		else if(app.timers[i] != -1) // timers set to -1 are stopped
		{
			if(app.timer_intervals[i])
			{
				uint16_t time_remainder = time_elapsed - app.timers[i];
				RELOAD_TIMER(i);
				app.timers[i] -= time_remainder % app.timer_intervals[i];
			}
			else app.timers[i] = -1; // stop the timer if the interval is 0
				
			if(TIMER_CALLBACK(i) != NULL)
				TIMER_CALLBACK(i)();
		}
	}
	// handle 1mS update functions
	refresh_screen_msg_ms(time_elapsed);
	pollMotionSensor();
}

void screenSaver(void)
{
	screen_active_mode(FALSE);
}

void pollLightSensor(void)
{
	app.lux = VEML6030_readLight();
	volatile uint32_t brightness = app.lux;
	if(brightness > 1200) brightness = 1200;
	//((((au32_IN - au32_INmin)*(au32_OUTmax - au32_OUTmin))/(au32_INmax - au32_INmin)) + au32_OUTmin);
	brightness = ((((brightness)*(50 - 20))/(1200)) + 20);
	app.brightness = (uint8_t)brightness;
	setBrightness(app.brightness);
	
	RELOAD_TIMER(LT_SENSE_TICK);
}

void pollHumidityTempSensor(void)
{
	app.temperature = Si7021_readTemperature();
	app.humidity = Si7021_readHumidity();
}

void printSensorDebug(void)
{
	if(!app.time_config.rtc_set) return; // if we haven't finished initializing, don't congest the terminal
	
	char time_str[30] = {0};
	getLocalTimeStr(time_str, sizeof(time_str));
	printf("Time: %s\n", time_str);
	
	if(app.display_fahrenheit)
		printf("Temperature: %.2fF\nHumidity: %.2f\nLight: %lu Lux\n\n", ((app.temperature * 9.0/5.0) + 32), app.humidity, app.lux);
	else
		printf("Temperature: %.2fC\nHumidity: %.2f\nLight: %lu Lux\n\n", app.temperature, app.humidity, app.lux);
}

// sets the environment variables used by time.h to local (stored in app as a string) or UTC
void setTimeEnvLocal(bool set_local)
{
	/*
	Note: Due to memory leak issues with the setenv functions, we have to ensure that when changing the value of the
	environment variable TZ, we keep the length of the value the same. We accomplish this by 0 padding the tail end
	of the value. Space padding caused issues with UTC0, this solution assumes the local time zone's rules are tolerant
	of zero padding but this may not be true!
	
	Eg. "TZ=EST+5EDT,M3.2.0,M11.1.000000000"
		"TZ=UTC0000000000000000000000000000"
	
	Debug notes:
	SRAM address space is 0x20000000 through 0x20007FFF 
	memory leak seen around address 0x200038A8
	normally the tz string should only be seen around 0x20003149 - obviously this is application dependent
	*/
	#define TZ_ENV_VAR_LEN_WITH_PADDING 40
	#define TZ_ENV_PADDING_CHAR '0'
	
	char environment_variable[TZ_ENV_VAR_LEN_WITH_PADDING] = {0};
	memset(environment_variable, TZ_ENV_PADDING_CHAR, TZ_ENV_VAR_LEN_WITH_PADDING - 1); // default to all zeros except last byte
	if(set_local && strlen(app.time_config.time_zone) != 0)
		memcpy(environment_variable, app.time_config.time_zone, strlen(app.time_config.time_zone));
	else
		memcpy(environment_variable, "UTC0", strlen("UTC0"));
	
	setenv("TZ", environment_variable, 1);
	tzset();
}

// string provided to this function must be at least 25 bytes
void getLocalTimeStr(char* time_str, uint8_t len)
{
	assert(len >= 25);
	
	/* Get current time from RTC */
	struct rtc_calendar_time rtc_time;
	rtc_calendar_get_time(&rtc_instance, &rtc_time);
	
	// create string that we can parse with time.h utilities
	char rtc_string[40] = {0};
	sprintf(rtc_string, "%02d/%02d/%d %02d:%02d:%02d", rtc_time.month, rtc_time.day, rtc_time.year, rtc_time.hour, rtc_time.minute, rtc_time.second);
	
	setTimeEnvLocal(false); // read the time as UTC
	struct tm utc_time;
	strptime(rtc_string, "%m/%d/%Y %H:%M:%S", &utc_time);
	time_t unix_epoch_time = mktime(&utc_time);
	
	setTimeEnvLocal(true); // print the time as local
	strftime(time_str, len, "%a %D %r %Z", localtime(&unix_epoch_time));
}

void markRtcSet(void)
{
	app.time_config.rtc_set = true;
}

bool isRtcSet(void)
{
	return app.time_config.rtc_set;
}

// set a specified app timer
void setTimer(app_timers_t timer, uint32_t value)
{
	SET_TIMER_INTERVAL(timer, value);
}

// callback handler needed to provide app context to remote
void remoteSyncCallback(void)
{
	remoteSync(app);
}

// callback handler needed to provide app context to remote for wifi connection
void wifiReconnectCallback(void)
{
	app.remote.wifi_failed_connect_attempts++;
	printf("Reconnecting to WiFi - attempt number: %d\n", app.remote.wifi_failed_connect_attempts);
	connectToWifi(app.remote);
}

// verify we're still connected
void wifiCheckConnectionStatus(void)
{
	if(app.remote.wifi_connected)
		setWifiConnectionState(isWifiConnected() && WiFiClient_status()); //wificlient status being 0 appears to correlate to a connection issue, let's try rebooting the radio
}

void wifiFailedToConnect(void)
{
	RELOAD_TIMER(WIFI_RECONNECT_TICK);
}

void setWifiConnectionState(bool connected)
{
	if(connected && !app.remote.wifi_connected) // just established connection
	{
		app.remote.wifi_connected = true;
		app.remote.wifi_connect_count++;
		STOP_TIMER(WIFI_RECONNECT_TICK);
		printf("Connected to WiFi!\n");
		//getTime(); // as soon as we connect to WiFi, update the system time
		static bool once = false;
		if(!once)
		{
			once = true;
			getTime();
		}
	}
	if(!connected && app.remote.wifi_connected) // just lost connection
	{
		app.remote.wifi_connected = false;
		app.remote.wifi_failed_connect_attempts = 1;
		RELOAD_TIMER(WIFI_RECONNECT_TICK);
		printf("Lost WiFi Connection! - Reconnecting...\n");
		connectToWifi(app.remote);
	}
}

void pollMotionSensor(void)
{
	
	if(port_pin_get_input_level(PIR_SENSOR_PIN) == HIGH)
	{
		port_pin_set_output_level(STAT_LED_PIN, HIGH);
		RELOAD_TIMER(SCREEN_ON_TICK);
		screen_active_mode(TRUE);
	}
	else
	{
		port_pin_set_output_level(STAT_LED_PIN, LOW);
	}
}