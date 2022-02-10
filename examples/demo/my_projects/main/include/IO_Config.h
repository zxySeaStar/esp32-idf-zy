#ifndef IO_CONFIG_H_
#define IO_CONFIG_H_

#include "driver/gpio.h"

#define GPIO_OUTPUT_IO_0        33
#define GPIO_OUTPUT_IO_1        34
#define SWCLK_TCK_PIN_PORT      37
#define SWDIO_OUT_PIN_PORT      38
#define SWDIO_IN_PIN_PORT       38
#define nRESET_PIN_PORT         36


#define GPIO_HIGH               0x1
#define GPIO_LOW                0x0

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))

#endif 