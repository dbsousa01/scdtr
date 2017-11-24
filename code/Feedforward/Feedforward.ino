#include <string.h>
#include <math.h>
#include <stdlib.h>

#define LED 3
#define LDR A0
#define numChar 140
  
float b = 1.808;
float m = -0.692;
int R1 = 10;
int LEDvalue, step_number;

int media = 0;

unsigned long delta_t1 , delta_t2;
unsigned long time_int = 500000;
unsigned long t_trans;

unsigned long sample_time = 4000;

double  AR , pwm , lux_lido;

double luximeter (double readval){  // Converts to luximeter

  double F = (R1*(5.0-readval*5.0/1023.0))/(readval*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
 
  return(lux_lido);
}
 
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}


bool newData = false;
char buf[numChar];
char refString[15] = "set lux ref:";
char feedString[15] = "set feed:";
int refValue = 0; //CHANGE THIS TO A BETTER STANDARD VALUE
int feedValue = 0; //CHANGE THIS TO A BETTER STANDARD VALUE
int refnow = 0;

int PWM_ref_low = 50;
int PWM_ref_high = 180;

int read_string(char *buf){
  static byte ndx;
  char rc;
  
  if(Serial.available() > 0){
    while(Serial.available() > 0){
      rc = Serial.read();
      Serial.println(rc);
      if(rc != '\n' && rc != '\r' && rc != 0){
        buf[ndx] = rc;
        ndx++;
        if(ndx >= numChar) //in case the string is too big he doesn't care about the later chars, we can change it to dynamic if you want.
          return 1;
      }else{
        buf[ndx] = '\0'; //terminates the string
        ndx = 0;
        newData = true;
        Serial.print("buffer: ");
        Serial.println(buf);
        return 1; //it read, return 1
      }
    }
  }
  return 0; //It did not read anything, return 0
}

int parseData() { //to do this I need to know how many strings are we receiving and what
    // split the data into its parts
    char * strtokIndx;
    
    if(!newData)
      return -1; //There is nothing to compare, proceed.

    if(strstr(buf, refString) != NULL){ // meaning it has read the refString
      Serial.println("Entrei no refString");
      strtokIndx = strtok(buf, ":");
      strtokIndx = strtok(NULL,":");
      if(strtokIndx == NULL){
        Serial.println("Oops something went wrong");
        return -1; //Something went wrong  
      }
      refValue = atoi(strtokIndx);
      Serial.print("Li uma ref de");
      Serial.println(refValue);
      newData = false;
      return 1; //read something valuable
    }else if(strstr(buf, feedString) != NULL){
      Serial.println("Entrei no feedString");
      strtokIndx = strtok(buf, ":");
      strtokIndx = strtok(NULL,":");
      if(strtokIndx == NULL){
        Serial.println("Oops something went wrong");
        return -1; //Something went wrong  
      }
      feedValue = atoi(strtokIndx);
      Serial.print("Li um feed de");
      Serial.println(feedValue);
      newData=false;
      return 1; //read something valuable
    }else{
      newData=false;
      return 0; //I read something but it was garbage
    }
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  
  setPwmFrequency(LED, 1);

  LEDvalue = PWM_ref_low;
  analogWrite(LED, LEDvalue);
  delay(1000);  // allow things to stabilize
  Serial.println("Beginnig of the loop");
}


// the loop routine runs over and over again forever:
void loop() {
  
  delta_t1 = micros();

  for(int j = 0 ; j < 5 ; j++){ // Reads 5 values and does the mean to reduce noise
      //delay(SampleDelay); // Introduces a delay so you can read 5 values per sample - Need to calculate
      AR = analogRead(LDR);
      media += AR;
      delay(10);
  }
    
  media = media/5;
     
  lux_lido = luximeter(media);
  
  Serial.print(lux_lido);
  Serial.println(";");
  delay(500);
  media = 0;
  
  int q = read_string(buf);//reads serial monitor
  int z = parseData();
  
  if (refnow == refValue){    
    
    delta_t2 = micros();
    delayMicroseconds(sample_time-(delta_t2-delta_t1)); 
    
  }else{
    step_number = feedValue*(time_int/sample_time);

    if(step_number == 0){
      LEDvalue = LEDvalue + (refValue-refnow)*(PWM_ref_high - PWM_ref_low);
      analogWrite(LED, LEDvalue);
      
      delta_t2 = micros();
      delayMicroseconds(sample_time-(delta_t2-delta_t1)); 
      
    }else{
      delta_t2 = micros();
      delayMicroseconds(sample_time-(delta_t2-delta_t1)); 
      
      for (int i = 1 ; i <= step_number ; i++){
        delta_t1 = micros();
        
        AR = analogRead(LDR);
        lux_lido = luximeter(AR);
       
        Serial.print(lux_lido);
        Serial.println(";");
       
        LEDvalue = LEDvalue + (refValue-refnow)*(PWM_ref_high - PWM_ref_low)/(step_number);
        analogWrite(LED, LEDvalue);
        delta_t2 = micros();
        delayMicroseconds(sample_time-(delta_t2-delta_t1));
      }
    }          
    refnow = refValue;
  }
}
