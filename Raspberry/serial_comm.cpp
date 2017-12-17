#include <unistd.h> 
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>


#define BAUDRATE 115200
#define PORT "/dev/ttyUSB0"

int main(){

    int fd = open(PORT, O_RDWR);
    if (fd == -1) {
      perror("/dev/ttyUSB0");
      return 1;
    }

    struct termios tios;
    tcgetattr(fd, &tios);
    // disable flow control and all that, and ignore break and parity errors
    tios.c_iflag = IGNBRK | IGNPAR;
    tios.c_oflag = 0;
    tios.c_lflag = 0;
    cfsetspeed(&tios, B115200);
    tcsetattr(fd, TCSAFLUSH, &tios);

    // the serial port has a brief glitch once we turn it on which generates a
    // start bit; sleep for 1ms to let it settle
    usleep(1000);    

    // output to serial port
    char msg[] = "hi there";
    write(fd, msg, strlen(msg));
    char c;
    for(;;){
    	if(read(fd,&c,1)>0){
    		fprintf(stdout,"%c\n", c);	
    	}    	
    }
}