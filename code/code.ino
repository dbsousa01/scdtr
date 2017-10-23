#include <math.h>
#include <stdlib.h>

#define LED 3
#define LDR A0

#define MaxSamples 5
#define SampleDelay 1000

float b = 1.808;
float m = -0.692;
int R1 = 10;

double lux_lido;
double  AR;
double media;

int flag = 0;

unsigned long time1 , time2;

int i , j;

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


double luximeter (double readval){  // Converts to luximeter

  double F = (R1*(5.0-readval*5.0/1023.0))/(readval*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
 
  return(lux_lido);
}

int read_string(char *buf){

  if(Serial.available()<=0) // There is nothing to read, reutnr -1
    return -1;

  while(Serial.available()>0){ // while there is stuff to read
    b = Serial.read();

    if(b == 0 || b == '\n' || b == '\r')
      break;
      
    *buf = b;
    *(buf++);//passes on to the next char of the string

    delay(1);
  }

  *buf='\0'; //Finalizes buffer with a string finalizer
  Serial.println(buf); // Prints what it was read, for debugging reasons
  return 1; // reading is finished, buf has everything that its read, return 1
}
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);

  setPwmFrequency(LED,1);
  

}


// the loop routine runs over and over again forever:
void loop() {
  
  for (i = 0 ; i<=250 ; i = i + 10){ //PWM stair    

    time1 = millis(); //captiures the time of initialization of the pwm step
    analogWrite(LED,i);
    delay(50); //Waits a certain time for stability in the pwm value before reading
    time2 = millis();
    while((time2-time1) <= 100){
      time2 = millis();    
      for(j = 0 ; j < MaxSamples ; j++){ // Reads 5 values and does the mean to reduce noise
      //delay(SampleDelay); // Introduces a delay so you can read 5 values per sample - Need to calculate
      AR = analogRead(LDR);
      media += AR;
      }
    
      media = media/MaxSamples;
      
      lux_lido = luximeter(media);

      

      Serial.print(media);
      Serial.print(",");
      Serial.print(i);
      Serial.print(",");
      Serial.print(lux_lido);
      Serial.println(";");
  
      media = 0;    
      //time2 = millis(); //captures the time of the finishing of the computation
      //delay(100-(time1-time2)); //guarantees that the pwm step is 0.1 sec long
      
    }
  }
  
}

