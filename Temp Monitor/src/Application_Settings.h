/*
 * Application_Settings.h
 *
 * Created: 4/27/2022 3:21:36 AM
 *  Author: Ben
 */ 


#ifndef APPLICATION_SETTINGS_H_
#define APPLICATION_SETTINGS_H_

///////////////////////////// VERSION INFO /////////////////////////////
#define FIRMWARE_VERSION "0.1.0" // major.minor.patch
#define RECORD_VERSION "2"

///////////////////////////// CONNECTION INFO /////////////////////////////
#define PROD_BE_SERVER "iot.benrawstron.org"
#define NTP_TIME_SERVER "time.nist.gov"

///////////////////////////// TIMER DEFINES /////////////////////////////
#define DEFAULT_SLOW_LT_SENSE_POLL_INTERVAL			2000
#define DEFAULT_FAST_LT_SENSE_POLL_INTERVAL			35
#define DEFAULT_HUMIDITY_TEMP_SENSE_POLL_INTERVAL	1000
#define DEFAULT_SCREEN_ON_DURATION					20000
#define DEFAULT_SENSOR_DEBUG_INTERVAL				2000
#define DEFAULT_TIME_SYNC_INTERVAL					3600000 //43200000 // 12 hours, 1 hour for now due to rtc drift issues...
#define DEFAULT_REMOTE_SYNC_INTERVAL				5000

#define TIME_SYNC_RETRY_INTERVAL					30000 // 30 seconds
#define WIFI_CHECK_STAT_INTERVAL					5000  // 5 second
#define WIFI_RECONNECT_INTERVAL						15000 // 15 seconds


#endif /* APPLICATION_SETTINGS_H_ */