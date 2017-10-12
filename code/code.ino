#include <math.h>
#include <stdlib.h>

#define LED 13
#define LDR A0

#define MaxSamples 5
#define SampleDelay 1000

float b = 1.808;
float m = -0.692;
int R1 = 10;
  
double luximeter (double readval){  // Converts to luximeter

  double F = (R1*(5.0-readval*5.0/1023.0))/(readval*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
 
  return(lux_lido);
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  
}

// the loop routine runs over and over again forever:
void loop() {
  double lux_lido;
  double  AR;
  double media;
  
  digitalWrite(LED, HIGH);
  delay(1000);                       // wait
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait
  
  for(int i = 0; i< MaxSamples; i++){ // Reads 5 values and does the mean to reduce noise
    AR = analogRead(LDR);
    media += AR;
    delay(SampleDelay); // Introduces a delay so you can read 5 values per sample - Need to calculate
  }
  media = media/MaxSamples;
  lux_lido = luximeter(media);
  
  Serial.print(lux_lido);
  delay(500);        // delay in between reads for stability
}

