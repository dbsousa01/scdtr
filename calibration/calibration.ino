
#include <Wire.h>
#include <EEPROM.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define LED 3
#define LDR A0
#define numChar 140

const int EPROM = 0;	//the other device address
const int self_add = EEPROM.read(EPROM);

double l_i , k_ii , k_ij , o;
double di , dj;

int send_add = -self_add + 3;

char com_buf[5];
int buf_idx = 0;
float buf_num;

float b;
float m = -0.692;
int R1 = 10;


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

void receiveEvent(int howMany){
  while(Wire.available() > 0){
    char c_rec = Wire.read();	// receive byte as a character
    if (buf_idx == 3){
      com_buf[buf_idx] = c_rec;
      buf_num = atof(com_buf);
      buf_idx++;
    }else if(buf_idx == 4){buf_idx = 0;}
     else{
      com_buf[buf_idx] = c_rec;
      buf_idx++;
    }
    
  }
}


double luximeter (double readval){  // Converts to luximeter
  
  double F = (R1*(5.0-readval*5.0/1023.0))/(readval*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
 
  return(lux_lido);
}



void calibration(){
    
   delay(1000); 
   di = 255 * (self_add -1);
   
   int val = analogRead(LDR);
      
   o = luximeter(val);
   
   delay(100);
   
   analogWrite(LED,di);
   
   delay(500);
   
   val = analogRead(LDR);
   
   l_i = luximeter(val);
   
   Serial.print("Valor lido para o k_ij: ");
   Serial.println(l_i);

   if (di == 0){
     k_ij = (l_i - o)/100;
   }else if (di == 255){
     k_ii = (l_i - o)/100;     
   }  

  delay(500);
   
  //---------------------LED CHANGE---------------------// 
   
   di = 255 * (-self_add + 2);

   analogWrite(LED,di);
   
   delay(500);
   
   val = analogRead(LDR);
   
   l_i = luximeter(val);
   
   Serial.print("Valor lido para o k_ii: ");
   Serial.println(l_i);    
   
   if (di == 0){
     k_ij = (l_i - o)/100;
   }else if (di == 255){
     k_ii = (l_i - o)/100;     
   }
   
}
 

void setup(){
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  
  setPwmFrequency(LED, 1);
  
  
  if(self_add == 1){
    b = 1.758;
  }else{
    b = 1.808;
  }
  
  analogWrite(LED, 0);
  Serial.println("Beginning of the program");
  Serial.print("Address of this luminaire: ");
  Serial.println(self_add);
  delay(1000);  // allow things to stabilize
  Wire.begin(self_add);	 //join as a master/slave at the address saved in the EPROM
  Wire.onReceive(receiveEvent);	//register event handler
  
  calibration();
  analogWrite(LED, 0);
  delay(100);  // allow things to stabilize again
  Serial.println("Calibration concluded");
  Serial.print("Value of o: ");
  Serial.println(o);
  Serial.print("Value of kii: ");
  Serial.println(k_ii);
  Serial.print("Value of kij: ");
  Serial.println(k_ij);
      
}


void loop(){
  
  
    
}

