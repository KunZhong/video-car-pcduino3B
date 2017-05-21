#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "Arduino.h"

unsigned long millis(void);
unsigned long micros(void);

void delay(unsigned long);
void delayMicroseconds(unsigned int us);

void delaySched(unsigned long);
void delayMicrosecondsSched(unsigned int us);

void pabort(const char *s);


#endif
