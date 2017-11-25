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

double li , kii , kij , o;

int di , dj;

int send_add = -self_add + 3;

char com_buf[8];
char aux_buf[5];
int buf_idx = 0;
float buf_num;

float b;
float m = -0.692;
int R1 = 10;

float dii , dij , djj , dji;

float u1 = o-ref;
float u2 = 0;
float u3 = 100;

float pii = 1/(rho + qi);
float pij = 1/rho;

float zii = -ci - yii + rho*dii_av;
float zij = -yij + rho*dij_av;

float n = kii*kii*pii + kij*kij*pij;
float w1 = -kii*zii*pii - kij*zij*pij;
float w2 = -zii*pii;
float w3 = -w2;


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
      
      for (int i = 0; i < 5; i = ++) {
        aux_buf[i] = com_buf[i];
      }
      
      djj = atof(aux_buf);
      buf_idx++;
    }else if(buf_idx == 4){buf_idx = 5;}
    else if(buf_idx == 8){
      
      com_buf[buf_idx] = c_rec;
      for (int i = 0; i < 5; i = ++) {
        aux_buf[i] = com_buf[i+5];
      }
      
      dji = atof(aux);
      buf_idx++;
    }else if(buf_idx == 9){buf_idx = 0;}
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
   
   li = luximeter(val);
   
   Serial.print("Valor lido para o kij: ");
   Serial.println(li);

   if (di == 0){
     kij = (li - o)/100;
   }else if (di == 255){
     kii = (li - o)/100;     
   }  

  delay(500);
   
  //---------------------LED CHANGE---------------------// 
   
   di = 255 * (-self_add + 2);

   analogWrite(LED,di);
   
   delay(500);
   
   val = analogRead(LDR);
   
   li = luximeter(val);
   
   Serial.print("Valor lido para o kii: ");
   Serial.println(li);    
   
   if (di == 0){
     kij = (li - o)/100;
   }else if (di == 255){
     kii = (li - o)/100;     
   }
   
}

void send_values(add,value1,value2){
  
  Wire.beginTransmission(add);	
  Wire.write(value1);
  Wire.write(value2);	
  Wire.endTransmission();
  
}

void feasibility_check(a,b){
  
  if(a < 0 || a > 100 || kii*a + kij*b < L-o)
    sol_un = 0;
  if(sol_un){

  calc_min = 0.5*qi*pow(a,2) + ci*a + yii*(a - dii_av) + yij*(b - dij_av) + 0.5*rho*pow((a - dii_av),2) + 0.5*rho*pow((b - dij_av),2);
  
    if(calc_min < best_min_i(K){
       
      dii_best = a;
      dij_best = b;
      best_min_i(K) = calc_min;
    
    }  
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
  Serial.println(kii);
  Serial.print("Value of kij: ");
  Serial.println(kij);
      
}

void loop(){
  
  for (int K = 0, K < 50, K++){
  
    //unconstraint min:
    
    dii_un = pii*zii;
    dij_un = pij*zij;
    
    //Feasibility check:
    
    feasibility_check(dii_un,dij_un);
    
    //Min constraint to linear boundary:
    
    dii_lb = pii*zii + pii*kii/n*(w1-u1);
    dij_lb = pij*zij + pij*kij/n*(w1-u1);
    
    //Feasibility check:
    
    feasibility_check(dii_lb,dij_lb);
    
    //Compute minimum constrained to 0 boundary
    
    dii_b0 = 0;
    dij_b0 = pij*zij;
    
    //Feasibility check:
    
    feasibility_check(dii_b0,dij_b0);
    
    //Compute minimum constrained to 100 boundary
    dii_b100 = 100;
    dij_b100 = pij*zij;
    
    //Feasibility check:
    
    feasibility_check(dii_b100,dij_b100);
    
    //Compute minimum constrained to linear and zero boundary
    common = (rho+qi)/((rho+qi)*n-kii*kii);
    det1 = common;
    det2 = -kii*common;
    det3 = det2;
    det4 = n*(rho+qi)*common;
    x1 = det1*w1 + det2*w2;
    x2 = det3*w1 + det4*w2;
    v1 = det1*u1 + det2*u2; //u2 = 0 so this can be simplified
    v2 = det3*u1 + det4*u2; //u2 = 0 so this can be simplified
    dii_l0 = pii*zii+pii*kii*(x1-v1)+pii*(x2-v2);
    dij_l0 = pij*zij+pij*kij*(x1-v1);
    
    //Feasibility check:
    
    feasibility_check(dii_l0,dij_l0);
    
    //Compute minimum constrained to linear and 100 boundary
    common = (rho+qi)/((rho+qi)*n-kii*kii);
    det1 = common;
    det2 = k11*common;
    det3 = det2;
    det4 = n*(rho+qi)*common;
    x1 = det1*w1 + det2*w3;
    x2 = det3*w1 + det4*w3;
    v1 = det1*u1 + det2*u3; 
    v2 = det3*u1 + det4*u3; 
    dii_l100 = pii*zii+pii*kii*(x1-v1)-pii*(x2-v2);
    dij_l100 = pij*zij+pij*kij*(x1-v1);
    
    
    //Feasibility check:
    
    feasibility_check(dii_l100,dij_l100);
    
    //Store data and save for next cycle
    best_dii(K) = dii_best;
    best_dij(K) = dij_best;
    
    
    //compute average with available knowledge
    
    dii_av = (dii+djj)/2;
    dij_av = (dij+dji)/2;
    
    //update local lagrangian
    
    yii = yii + rho*(dii-dii_av);
    yij = yij + rho*(dij-dij_av);
    
    //Send node i solution to neighboor
    
    send_values(send_add,dii,dij);
    
    
  }
} 
