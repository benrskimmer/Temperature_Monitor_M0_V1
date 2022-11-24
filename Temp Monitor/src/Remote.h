/*
 * Remote.h
 *
 * Created: 4/11/2022 2:33:00 AM
 *  Author: Ben
 */ 


#ifndef REMOTE_H_
#define REMOTE_H_

#include <stdio.h>
#include <inttypes.h>

#include "WiFiNINA.h"
#include "Application.h"

BOOL remoteInit(remote_context_t remote_config);

void remoteSync(app_state_t app);

void getTime(void);

void runRemote(void);

void connectToWifi(remote_context_t remote_config);

bool isWifiConnected(void);

#endif /* REMOTE_H_ */