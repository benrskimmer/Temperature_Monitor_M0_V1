/*
 * screen_msg_buffer.h
 *
 * Created: 1/19/2022 9:53:22 PM
 *  Author: Ben
 */ 


#ifndef SCREEN_MSG_BUFFER_H_
#define SCREEN_MSG_BUFFER_H_

#include <inttypes.h>
#include <malloc.h>
#include "compiler.h"


// ------- settings -------
#define MAX_MSGS 20 // max number of messages the library can queue up. memory isn't allocated until a message is created
#define MAX_MSG_LEN 120 // max length of message string in characters
#define LOWEST_MSG_PRIORITY 4 // P0 is the highest priority message



// ------- data types -------
typedef struct scrn_msg
{
	bool used;
	bool active;
	int8_t cycles_left;
	uint8_t display_time;
	uint8_t priority;
	char* message;
	
	// used by message queue
	struct scrn_msg* previous;
	struct scrn_msg* next;
} scrn_msg_t;


// ------- public functions -------

scrn_msg_t* new_message(char* msg_str, uint8_t priority);
void delete_message(scrn_msg_t* msg);
void print_message(char* msg_str);
void print_message_repeat(char* msg_str, uint8_t repeat_cycles, uint8_t priority);

void update_priority(scrn_msg_t* msg, uint8_t priority);
void update_repeat(scrn_msg_t* msg, uint8_t repeats);
//void set_first(scrn_msg_t* msg); // sets this message to display first on wakeup, then normal priority takes over

void refresh_screen_msg(void); // call this in 1mS superloop



#endif /* SCREEN_MSG_BUFFER_H_ */