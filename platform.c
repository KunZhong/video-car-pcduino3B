#include "platform.h"

unsigned long millis()
{
    struct timeval tv; 
    gettimeofday (&tv , NULL); 
    return (tv.tv_sec*1000 + tv.tv_usec/1000);
}

unsigned long micros() {
    struct timeval tv;
        gettimeofday (&tv , NULL);
    return (tv.tv_sec * 1000000 + tv.tv_usec);
}

void delay(unsigned long ms)
{
    usleep(ms*1000);
}

void delayMicroseconds(unsigned int us)
{
    usleep(us);
}

void delaySched(unsigned long ms)
{
    usleep(ms*1000);
}

void delayMicrosecondsSched(unsigned int us)
{
    usleep(us);
}

void pabort(const char *s)
{
    perror(s);
    abort();
}
