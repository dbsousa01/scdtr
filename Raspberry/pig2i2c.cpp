/*
*/

#include "pig2i2c.h"
#include "globals.h"

/*
This software reads pigpio notification reports monitoring the I2C signals.

Notifications are pipe based so this software must be run on the Pi
being monitored.

It should be able to handle a 100kHz bus.  You are unlikely to get any
usable results if the bus is running at 400kHz.

gcc -o pig2i2c pig2i2c.c

Do something like

sudo pigpiod -s 2

# get a notification handle, assume handle 0 was returned

pigs no

# start notifications for SCL/SDA

e.g. pigs nb 0 0x3   # Rev. 1 select gpios 0/1
e.g. pigs nb 0 0xC   # Rev. 2 select gpios 2/3
e.g. pigs nb 0 0xA00 # select gpios 9/11 (1<<9|1<<11)

# run the program, specifying SCL/SDA and notification pipe

./pig2i2c SCL SDA </dev/pigpioN # specify gpios for SCL/SDA and pipe N

e.g. ./pig2i2c 1  0 </dev/pigpio0 # Rev.1 I2C gpios
e.g. ./pig2i2c 3  2 </dev/pigpio0 # Rev.2 I2C gpios
e.g. ./pig2i2c 9 11 </dev/pigpio0 # monitor external bus 
*/

static char * timeStamp()
{
   static char buf[32];

   struct timeval now;
   struct tm tmp;

   gettimeofday(&now, NULL);

   localtime_r(&now.tv_sec, &tmp);
   strftime(buf, sizeof(buf), "%F %T", &tmp);

   return buf;
}

void parse_I2C(int SCL, int SDA, int* n,int*idx, int*flag_message,char *msg)
{
   static int in_data=0, byte=0, bit=0;
   static int oldSCL=1, oldSDA=1;

   int xSCL, xSDA;


   if (SCL != oldSCL)
   {
      oldSCL = SCL;
      if (SCL) xSCL = SCL_RISING;
      else     xSCL = SCL_FALLING;
   }
   else        xSCL = SCL_STEADY;

   if (SDA != oldSDA)
   {
      oldSDA = SDA;
      if (SDA) xSDA = SDA_RISING;
      else     xSDA = SDA_FALLING;
   }
   else        xSDA = SDA_STEADY;

   switch (xSCL+xSDA)
   {
      case SCL_RISING + SDA_RISING:
      case SCL_RISING + SDA_FALLING:
      case SCL_RISING + SDA_STEADY:
         if (in_data)
         {
            if (bit++ < 8)
            {
               byte <<= 1;
               byte |= SDA;
            }
            else
            {
               if((*n)!=0){
                  unsigned char c_byte = (char)byte;
                  //sprintf(c_byte, "%x", byte);
                  //printf("%c", c_byte);
                  if(*flag_message == 1){
                     msg[*idx] = c_byte;
                     (*idx)++;
                  }
                  if(c_byte == 'M'){
                     (*flag_message) = 1;
                  }
                  //if (SDA) printf("-"); else printf("+");                  
               }
               bit = 0;
               byte = 0;
               (*n) = (*n) +1;
            }
         }
         break;

      case SCL_FALLING + SDA_RISING:
         break;

      case SCL_FALLING + SDA_FALLING:
         break;

      case SCL_FALLING + SDA_STEADY:
         break;

      case SCL_STEADY + SDA_RISING:
         if (SCL)
         {
            in_data = 0;
            byte = 0;
            bit = 0;

            //printf("]\n"); // stop
            msg[*idx] = '\0';
            if(strlen(msg) == *idx){
                  if(strncmp(msg,"1",1) == 0){
                  boost::unique_lock<boost::shared_mutex> lock(ard.at(0)->mutex_);
                  ard.at(0)->comms.push(msg);
                  //boost::unique_lock<boost::shared_mutex> unlock(ard.at(0)->mutex_);
               }else if(strncmp(msg,"2",1) == 0){
                  //printf("MESSAGE 0 POSITION:%s\n",msg[0]);
                  boost::unique_lock<boost::shared_mutex> lock(ard.at(1)->mutex_);
                  ard.at(1)->comms.push(msg);
                  //boost::unique_lock<boost::shared_mutex> unlock(ard.at(1)->mutex_);
               }
            }
            (*idx) = 0;
            (*flag_message) = 0;
         }
         break;

      case SCL_STEADY + SDA_FALLING:
         if (SCL)
         {
            in_data = 1;
            byte = 0;
            bit = 0;

            //printf("["); // start
            *n = 0;
         }
         break;

      case SCL_STEADY + SDA_STEADY:
         break;

   }
}

void read_sniffer()
{
   int gSCL, gSDA, SCL, SDA, xSCL;
   int r;
   uint32_t level, changed, bI2C, bSCL, bSDA;
   int n = 0, idx = 0, flag_message = 0;
   char msg[25];

   gpioReport_t report;

   int argc = 4;
   char * argv[]={"pig2i2c","3","2","</dev/pigpio0"};
 if (argc > 2)
   {
      gSCL = atoi(argv[1]);
      gSDA = atoi(argv[2]);

      bSCL = 1<<gSCL;
      bSDA = 1<<gSDA;

      bI2C = bSCL | bSDA;
   }
   else
   {
      exit(-1);
   }
   /* default to SCL/SDA high */

   SCL = 1;
   SDA = 1;
   level = bI2C;

   while ((r=read(STDIN_FILENO, &report, RS)) == RS)
   {
      report.level &= bI2C;

      if (report.level != level)
      {
         changed = report.level ^ level;

         level = report.level;

         if (level & bSCL) SCL = 1; else SCL = 0;
         if (level & bSDA) SDA = 1; else SDA = 0;

         parse_I2C(SCL, SDA, &n,&idx,&flag_message, msg);
      }
   }
}