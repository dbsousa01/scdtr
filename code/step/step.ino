#include <math.h>
#include <stdlib.h>

#define LED 6
#define LDR A0
  
float b = 1.808;
float m = -0.692;
int R1 = 10;

unsigned long delta_t1 , delta_t2;
unsigned long time_int = 100000 *10;
unsigned long step_interval = 1000000 *10;

unsigned long sample_time = 2000 *10;

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

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  
  setPwmFrequency(LED, 1);

  
  analogWrite(LED, 0);
  delay(1000);	// allow things to stabilize
  
}


// the loop routine runs over and over again forever:
void loop() {
  analogWrite(LED, 0);
  Serial.println("Beginnig of the step");
  for (int i = 1 ; i<=time_int/sample_time ; i++){
    
    delta_t1 = micros();
    
    AR = analogRead(LDR);
    lux_lido = luximeter(AR);
   
    Serial.print(lux_lido);
    Serial.println(";");
    
    delta_t2 = micros();
    delayMicroseconds(sample_time-(delta_t2-delta_t1));
  }    
  analogWrite(LED, 128);  
  for (int i = 1 ; i <= (step_interval-time_int)/sample_time ; i++){
    
    delta_t1 = micros();
    
    AR = analogRead(LDR);
    lux_lido = luximeter(AR);
   
    Serial.print(lux_lido);
    Serial.println(";");
    
    delta_t2 = micros();
    delayMicroseconds(sample_time-(delta_t2-delta_t1));
  }    
}
