#include <math.h>
#include <stdlib.h>

#define LED 13
#define LDR A0

float b = 1.808;
float m = -0.692;
int R1 = 10;

int compare (const void * a, const void * b){
    return (*(int *)a - *(int *)b);
}

double luximeter (double readval){ 

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

  /* for (int i=0;i==4;i++){
    AR[i] = analogRead(LDR);
    delay(10);    
  }  

  qsort(AR, 5, sizeof(double), compare);
  double AR_med = AR[2];
  for (int i=0;i==4;i++){
    Serial.println(AR[i]);
  }*/

  double  AR = analogRead(LDR);
  
  digitalWrite(LED, HIGH);
  delay(1000);                       // wait for a second
  lux_lido= luximeter(AR);
  //Serial.println(lux_lido);
  //digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  //delay(1);                       // wait for a second
  
  
  Serial.print(lux_lido);
  Serial.println(" lux");
  delay(500);        // delay in between reads for stability
  int val;
  Serial.read(val);
}

