#ifndef _WIRING_H
#define _WIRING_H

#include "Arduino.h"

#define MAX_PWM_NUM 5
#define MAX_PWM_LEVEL 255
#define MAX_GPIO_NUM 23

extern int gpio_pin_fd[MAX_GPIO_NUM+1];
extern int gpio_mode_fd[MAX_GPIO_NUM+1];
extern int pwm_fd[MAX_PWM_NUM+1];

void init(void);
#endif
