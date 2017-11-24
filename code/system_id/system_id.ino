#include <math.h>
#include <stdlib.h>

#define LED 3
#define LDR A0

double  AR, media;

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

void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);

  setPwmFrequency(LED,1);

  analogWrite(LED, 0);
  delay(1000);  // allow things to stabilize

}

void loop() {
  Serial.println("Beginning of the loop");
  for (int i = 0 ; i<=255 ; i++ ){ //PWM stair    
    
    analogWrite(LED,i);
    delay(100); //Waits a certain time for stability in the pwm value before reading
    for(int j = 0 ; j < 5 ; j++){ // Reads 5 values and does the mean to reduce noise
      AR = analogRead(LDR);
      media += AR;
    }
    media = media/5;
      
    //lux_lido = luximeter(media);

    Serial.print(media);
    Serial.print(",");
    Serial.print(i);
    Serial.println(";");
  
    media = 0;    
          
   }
}
