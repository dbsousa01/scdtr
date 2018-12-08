#ifndef PIG2I2C_H
#define PIG2I2C_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include "pigpio.h"

#define RS (sizeof(gpioReport_t))

#define SCL_FALLING 0
#define SCL_RISING  1
#define SCL_STEADY  2

#define SDA_FALLING 0
#define SDA_RISING  4
#define SDA_STEADY  8

static char * timeStamp();
void parse_I2C(int SCL, int SDA, int* n,int*idx, int*flag_message,char *msg);
void read_sniffer();

#endif 