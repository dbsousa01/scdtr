#include <math.h>
#include <stdlib.h>

#define LED 6
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

double luximeter (double readval){  // Converts to luximeter

  double F = (R1*(5.0-readval*5.0/1023.0))/(readval*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
 
  return(lux_lido);
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  

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

