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
#define MAX_MSGS 40 // max number of messages the library can queue up. memory isn't allocated until a message is created
#define MAX_MSG_LEN 800 // max length of message string in characters
typedef enum msg_priority // P0 is the highest priority message
{
	HIGHEST_MSG_PRIORITY = 0,
	MEDIUM_MSG_PRIORITY,
	LOWEST_MSG_PRIORITY,
	MAX_MSG_PRIORITY
} msg_priority_t;



// ------- data types -------
typedef struct scrn_msg
{
	bool used;
	bool active;
	int8_t cycles_left;
	uint16_t display_time_ms;
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
void update_message(scrn_msg_t* msg, char* msg_str);
//void set_first(scrn_msg_t* msg); // sets this message to display first on wakeup, then normal priority takes over
void screen_active_mode(bool enable);

void refresh_screen_msg_ms(uint16_t ms_elapsed); // call this in 1mS superloop with the number of mS elapsed - this provides us control over how many mS we can jump if the system binds momentarily



#endif /* SCREEN_MSG_BUFFER_H_ */