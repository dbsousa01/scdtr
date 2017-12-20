#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include <unistd.h> 
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>


#define BAUDRATE 115200
#define PORT "/dev/ttyACM0"

void serial_write(char * msg);

#endif