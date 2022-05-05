/*
 * millis.h
 *
 * Created: 8/24/2021 12:51:02 AM
 *  Author: Ben
 */ 


#ifndef MILLIS_H_
#define MILLIS_H_
#include <asf.h>

void millis_init(void);
uint32_t millis(void);
void delay(uint16_t mS);
uint16_t elapsedMS(void);

#endif /* MILLIS_H_ */