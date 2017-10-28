#include <string.h>
#include <math.h>
#include <stdlib.h>

#define LED 3
#define LDR A0
#define numChar 140
  
float b = 1.808;
float m = -0.692;
int R1 = 10;
int high = 180, low = 63;

int media = 0;

float step_number;
unsigned long delta_t1 , delta_t2;
unsigned long time_int = 500000;
unsigned long t_trans;

unsigned long sample_time = 2000;

double y = 0 , intg = 0 , intg_ant = 0 , e = 0 , e_ant = 0 , u = 0 , u_sat = 0 , p = 0 , K1 = 0 , K2 = 0;

double ref = low , ff = 0 , e_sat = 0 , windup = 0;

double Kp = 5 , Ki = 0.0000001 , Kff = 0, Kw = 0;

double  AR , pwm , lux_lido;

bool newData = false;
char buf[numChar];
char refString[15] = "set lux ref:";
char feedString[15] = "set feed:";
int refValue = 0; //CHANGE THIS TO A BETTER STANDARD VALUE
int feedValue = 1; //CHANGE THIS TO A BETTER STANDARD VALUE
int refnow = 0;

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

int read_string( ){
  static byte ndx;
  char rc;
  
  if(Serial.available() > 0){
    while(Serial.available() > 0){
      rc = Serial.read();
      if(rc != '\n' && rc != '\r' && rc != 0){
        buf[ndx] = rc;
        ndx++;
        if(ndx >= numChar) //in case the string is too big he doesn't care about the later chars, we can change it to dynamic if you want.
          return 1;
      }else{
        buf[ndx] = '\0'; //terminates the string
        ndx = 0;
        newData = true;
        return 1; //it read, return 1
      }
    }
  }
  return 0; //It did not read anything, return 0
}

int parseData() { //to do this I need to know how many strings are we receiving and what
    // split the data into its parts
    char * strtokIndx;
    int pendingInt = 3;
    
    if(!newData)
      return -1; //There is nothing to compare, proceed.

    if(strncmp(buf, refString,12) == 0){ // meaning it has read the refString
      strtokIndx = strtok(buf, ":");
      strtokIndx = strtok(NULL,":");
      if(strtokIndx == NULL){
        Serial.println("Oops something went wrong");
        return -1; //Something went wrong  
      }      
      pendingInt = atoi(strtokIndx);
      if(pendingInt != 0 && pendingInt != 1)
        return -1;
      Serial.println("Alteração de Referência");
      refValue = pendingInt;
      Serial.print("Referência introduzida: ");
      Serial.println(refValue);
      newData= false;
      return 1; //read something valuable
    }else if(strncmp(buf, feedString,9) == 0){
      strtokIndx = strtok(buf, ":");
      strtokIndx = strtok(NULL,":");
      if(strtokIndx == NULL){
        Serial.println("Oops something went wrong");
        return -1; //Something went wrong  
      }

      pendingInt = atoi(strtokIndx);
      if(pendingInt != 0 && pendingInt != 1)
        return -1;
      Serial.println("Alteração de Feedforward");
      feedValue = pendingInt;
      Serial.print("Estado do Feedforward: ");
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

  analogWrite(LED, 0);
  delay(1000);  // allow things to stabilize
  Serial.println("Beginning of the program");
}


// the loop routine runs over and over again forever:
void loop() {
  
  delta_t1 = micros();

  for(int j = 0 ; j < 5 ; j++){ // Reads 5 values and does the mean to reduce noise
      AR = analogRead(LDR);
      media += AR;
  }  
  y = media/5;
  lux_lido = luximeter(y);
  media = 0;
  
  int i = read_string( );//reads serial monitor
  i = parseData();
  
  if (refnow != refValue){    
    
    if(refValue == 1){
      ref = high;
    }else{
      ref = low;    
    }
    Serial.println("Mudou de referência");
    Serial.print("Referência = ");
    Serial.println(ref);  
    
    refnow = refValue;
  }    
  
  if(feedValue == 1){
    Kff = 0.1; 
  }else
    Kff = 0;    
  

  
  K1 = Kp * Kff;
  K2 = Kp * Ki;

  //Serial.print("y1 = ");
  //Serial.println(y);
  y = map(y,0,1023,0,255);
  e = ref - y;
  //Serial.print("y2 = ");
  //Serial.println(y);
  //Serial.print("e = ");
  //Serial.println(e);
  p = Kp*e;
  //Serial.print("p = ");
  //Serial.println(p);
  ff = Kff*ref;
  //Serial.print("ff = ");
  //Serial.println(ff);
  //Serial.print("e_sat = ");
  //Serial.println(e_sat);
  windup = Kw * e_sat;
  //Serial.print("windup = ");
  //Serial.println(windup);
  double K_aux = sample_time/2;
  //Serial.print("K_aux = ");
  //Serial.println(K_aux);
  intg = intg_ant + ((e + e_ant)*K_aux); //Discrete Approximation of the integral plus windup
  intg = K2 * intg;
  //Serial.print("intg = ");
  //Serial.println(intg);
  u = ff + p + intg;
  u_sat = constrain(u,0,255); //Saturates the u signal
  e_sat = u_sat - u; //Saturation diference
  intg_ant = intg;
  e_ant = e;

  //Serial.print("u = ");
  //Serial.println(u);
    
  //Serial.print("u_sat = ");
  //Serial.println(u_sat);
  
  analogWrite(LED,u_sat);
    
  Serial.print(refValue);
  Serial.print(",");
  Serial.print(lux_lido);
  Serial.print(",");
  Serial.print((u_sat/255)*100);
  Serial.println(";");
  
  delta_t2 = micros();
  /*
  if(sample_time-(delta_t2-delta_t1) > 0)
    Serial.println("OK");
  else
    Serial.println("NOT OK");
    */
  delayMicroseconds(sample_time-(delta_t2-delta_t1));
}
