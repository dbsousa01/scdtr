#include <Wire.h>
#include <EEPROM.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define LED 3
#define LDR A0
#define numChar 140

#define iterations 50
#define sample_time 3000

#define high 40
#define low 15
#define deadband 0.5

#define Kp 0.6
#define Ki Kp*0.0001
#define b_pi 1

#define EPROM 0
//..................................PI CONTROLLER CONSTS.......................................//

int media = 0;

//clock variables:
unsigned long delta_t1 , delta_t2;

double y = 0 , intg = 0 , intg_ant = 0 , e = 0 , e_ant = 0 , u = 0 , u_sat = 0, u_pwm_ant = 0 , p = 0 ;

double ref = low, ff = 0 , u_pwm = 0;

double K1 = Kp * b_pi;
double K2 = Kp * Ki * (sample_time / 2);

double  AR , pwm , lux_lido;

bool newData = false;
char buf[numChar];
char refString[15] = "set lux ref:";
char feedString[15] = "set feed:";
int refValue = 0;
int feedValue = 1;
int refnow = 0;

//....................................................................................//

//..................................CONSENSUS CONSTS.......................................//

const int self_add = EEPROM.read(EPROM);

double buf_num;

double kii , kij , o;

int send_add = -self_add + 3;

double b;
double m = -0.692;
int R1 = 10;

double dii=0 , dij=0 , djj=0 , dji=0 , dii_av = 0 , dij_av = 0;

double djj_ant = 1 , dji_ant = 1;

double dii_send = 0 , dij_send = 0;

double rho = 0.01;
double qi = 0.01 , qj = 0.01;
double ci = 1;
double yii = 0 , yij = 0;

int K;

volatile int flag_received = 0 , flag_calibration = 0 , flag_sent = 0 , flag_value = 0 , flag_consensus = 0;

double dii_best , best_dii[iterations];
double dij_best , best_dij[iterations];
double best_min_i[iterations];
int solution = 1;
double calc_min;

double dii_un = 0 , dij_un = 0 , dii_lb = 0 , dij_lb = 0 , dii_b0 = 0 , dij_b0 = 0 , dii_b100 = 0 , dij_b100 , dii_l0 , dij_l0 , dii_l100 , dij_l100;

double common = 0 , det1 = 0 , det2 = 0 , det3 = 0 , det4 = 0 , x1 = 0 , x2 = 0 , v1 = 0 , v2 = 0 ; 

double u1;
double u2;
double u3;

double pii;
double pij;

double zii;
double zij; 
double n;
double w1;
double w2;
double w3;

//....................................................................................//

//..................................SET PWM FREQ.......................................//

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

//....................................................................................//

//..................................WIRE RECEIVE.......................................//
void receiveEvent(int howMany){
  char c_rec;
  char com_buf[10+1+1];
  char * strtokaux1;
  int buf_idx=0;
  while(Wire.available() > 0){
    c_rec = Wire.read();  // receive byte as a character 
    if ( c_rec == 'a' ){
      flag_sent = 1;
      return;
    }else if (c_rec == 'c'){
      flag_calibration = 1;
      Serial.println(F("O outro acabou a calibração"));
      return;
    }else if (c_rec == 't'){
      flag_consensus = 1;
      Serial.println(F("O outro quer fazer consensus"));
      return;
    }else{
      com_buf[buf_idx] = c_rec;
      buf_idx++;
      flag_value = 1;

    }
  }

  if(flag_value == 1){
    com_buf[9] = '\0';
    Serial.println(com_buf);
      
    strtokaux1 = strtok(com_buf, ";");
    dji = atof(strtokaux1)*100;
    
    strtokaux1 = strtok(NULL, "\0");
    djj = atof(strtokaux1)*100;
    
    Serial.print(F("Valor de djj recebido: "));
    Serial.println(djj);
    Serial.print(F("Valor de dji recebido: "));
    Serial.println(dji);
    

    flag_received = 1;
    flag_value = 0;
  }
  
  
}
//....................................................................................//

//..................................LUXIMITER.......................................//

double luximeter (double readval){  // Converts to luximeter
  
  double F = (R1*(5.0-readval*5.0/1023.0))/(readval*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
 
  return(lux_lido);
}
//....................................................................................//

//..................................READ SERIAL.......................................//
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
        Serial.println(F("Oops something went wrong"));
        return -1; //Something went wrong  
      }      
      pendingInt = atoi(strtokIndx);
      if(pendingInt != 0 && pendingInt != 1)
        return -1;
      Serial.println(F("Alteração de Referência"));
      refValue = pendingInt;
      
      Serial.print(F("Referência introduzida: "));
      Serial.println(refValue);
      newData= false;
      return 1; //read something valuable
    }else if(strncmp(buf, feedString,9) == 0){
      strtokIndx = strtok(buf, ":");
      strtokIndx = strtok(NULL,":");
      if(strtokIndx == NULL){
        Serial.println(F("Oops something went wrong"));
        return -1; //Something went wrong  
      }

      pendingInt = atoi(strtokIndx);
      if(pendingInt != 0 && pendingInt != 1)
        return -1;
      Serial.println(F("Alteração de Feedforward"));
      feedValue = pendingInt;
      Serial.print(F("Estado do Feedforward: "));
      Serial.println(feedValue);
      newData=false;
      return 1; //read something valuable
    }else{
      newData=false;
      return 0; //I read something but it was garbage
    }
}
//....................................................................................//


//..................................CALIBRATION.......................................//

void calibration(){
    double li;
    flag_calibration = 0;
   delay(1000); 
  int di = 255 * (self_add -1);
   
   int val = analogRead(LDR);
      
   o = luximeter(val);
   
   delay(100);
   
   analogWrite(LED,di);
   
   delay(500);
   
   val = analogRead(LDR);
   
   li = luximeter(val);

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
   
   if (di == 0){
     kij = (li - o)/100;
   }else if (di == 255){
     kii = (li - o)/100;     
   }

  Serial.println(F("Calibration concluded"));
  Serial.print(F("Value of o: "));
  Serial.println(o);
  Serial.print(F("Value of kii (escala 100): "));
  Serial.println(kii);
  Serial.print(F("Value of kij (escala 100): "));
  Serial.println(kij); 

  char c = 'c'; 
  Wire.beginTransmission(send_add);
  Wire.write(c);
  Wire.endTransmission();
   
}
//....................................................................................//

//..................................SEND VALUES.......................................//

void send_values(double value1,double value2){
  char b1[4+1+1];
  char b2[4+1+1];
  
  char b[10+1+1];
  
  dtostrf(value1,3,2,b1);
  dtostrf(value2,3,2,b2);
  
  sprintf(b,"%s;%s;",b1,b2);

    
  Wire.beginTransmission(send_add);
  Wire.write(b);
  Wire.endTransmission();
  
  while (flag_received == 0){
    int x = 10*100;
  }
  flag_received = 0;
  
  
 
}
//....................................................................................//

//..................................FEASIBILITY CHECK.......................................//
void feasibility_check(double a,double b, char var){

  switch (var) {
  case 'a': //unconstraint
    if(a < 0 || a > 100 || kii*a + kij*b < ref-o)
    solution = 0;
    break;
  case 'b': //linear boundary
    if(a < 0 || a > 100)
    solution = 0;
    break;
  case 'c': //zero boundary
    if(a > 100 || kii*a + kij*b < ref-o)
    solution = 0;
    break;
  case 'd': //100 boundary
    if(a > 100 || kii*a + kij*b < ref-o)
    solution = 0;
    break;
  case 'e': //linear and zero boundary
    if(a > 100)
    solution = 0;
    break;
  case 'f': //linear and 100 boundary
    if(a < 0)
    solution = 0;
    break;
  default:
    break;
  }
  
  if(solution){

    calc_min = 0.5*qi*pow(a,2) + ci*a + yii*(a - dii_av) + yij*(b - dij_av) + 0.5*rho*pow((a - dii_av),2) + 0.5*rho*pow((b - dij_av),2);
  
    if(calc_min < best_min_i[K]){
       
      dii_best = a;
      dij_best = b;
      best_min_i[K] = calc_min;
    
    }  
  }
  solution = 1;
}
 
//....................................................................................//

//..................................CONSENSUS.......................................//

void consensus(){
  
  for (K = 0; K < iterations; K++){
      
      dii_best = -1;
      dij_best = -1;
      best_min_i[K] = 100000; //big number
      solution = 1;
      
      
      u1 = o-ref;
      u2 = 0;
      u3 = 100;
      
      pii = 1/(rho + qi);
      pij = 1/rho;
      zii = -ci - yii + rho*dii_av;
      zij = -yij + rho*dij_av;
      
      n = kii*kii*pii + kij*kij*pij;
      w1 = -kii*zii*pii - kij*zij*pij;
      w2 = -zii*pii;
      w3 = -w2;
      
      //unconstraint min:
      
      dii_un = pii*zii;
      dij_un = pij*zij;

      //Feasibility check:
      
      feasibility_check(dii_un,dij_un,'a');
            
      //Min constraint to linear boundary:
      
      dii_lb = pii*zii + pii*(kii/n)*(w1-u1);
      dij_lb = pij*zij + pij*(kij/n)*(w1-u1);
      
      //Feasibility check:
      
      feasibility_check(dii_lb,dij_lb,'b');
      
      //Compute minimum constrained to 0 boundary
      
      dii_b0 = 0;
      dij_b0 = pij*zij;
      
      
      //Feasibility check:
      
      feasibility_check(dii_b0,dij_b0,'c');

      
      //Compute minimum constrained to 100 boundary
      dii_b100 = 100;
      dij_b100 = pij*zij;
 
      //Feasibility check:
      
      feasibility_check(dii_b100,dij_b100,'d');
     
      
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

      //if(dii_l0 == -0.00) dii_l0 = 0.00;

      
      //Feasibility check:
      
      feasibility_check(dii_l0,dij_l0,'e');
      
      //Compute minimum constrained to linear and 100 boundary
      common = (rho+qi)/((rho+qi)*n-kii*kii);
      det1 = common;
      det2 = kii*common;
      det3 = det2;
      det4 = n*(rho+qi)*common;
      x1 = det1*w1 + det2*w3;
      x2 = det3*w1 + det4*w3;
      v1 = det1*u1 + det2*u3; 
      v2 = det3*u1 + det4*u3; 
      dii_l100 = pii*zii+pii*kii*(x1-v1)-pii*(x2-v2);
      dij_l100 = pij*zij+pij*kij*(x1-v1);

      
      //Feasibility check:
      
      feasibility_check(dii_l100,dij_l100,'f');

      

      //Store data and save for next cycle
      best_dii[K] = dii_best;
      best_dij[K] = dij_best;
      
      dii = dii_best;
      dij = dij_best;
      

      
      //compute average with available knowledge
      
      dii_av = (dii+djj)/2;
      dij_av = (dij+dji)/2;
      
      //update local lagrangian
      
      yii = yii + rho*(dii-dii_av);
      yij = yij + rho*(dij-dij_av);
      
      //Send node i solution to neighboor

      dii_send = dii/100;
      dij_send = dij/100;


      while (flag_calibration == 0){
        int x = 10*100;
      }

      char a = 'a'; 
      Wire.beginTransmission(send_add);
      Wire.write(a);
      Wire.endTransmission();

      while(flag_sent == 0){
        int x = 10*100;
        Serial.println(F("Quero Enviar!!!"));
      }
      flag_sent = 0;      
      send_values(dii_send,dij_send);
      
  }
  
  ref = kii*dii + kij*dij + o;
  Serial.print(F("Nova ref: "));
  Serial.print(ref);
  Serial.println(" LUX");
  Serial.print(F("Consensus (escala 0 a 100): "));
  Serial.print(dii);
  Serial.print(F(" ; ")); 
  Serial.println(dij); 

  flag_consensus = 0;
  
}

//....................................................................................//

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
  Serial.println(F("Beginning of the program"));
  Serial.print(F("Address of this luminaire: "));
  Serial.println(self_add);
  delay(1000);  // allow things to stabilize
  Wire.begin(self_add);   //join as a master/slave at the address saved in the EPROM
  Wire.onReceive(receiveEvent); //register event handler
  
  calibration();
  analogWrite(LED, 0);
  delay(100);  // allow things to stabilize again
 
  consensus();
      
}



void loop(){

  if(flag_consensus == 1){
    consensus();
  }
  
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
    char t = 't'; 
    Wire.beginTransmission(send_add);
    Wire.write(t);
    Wire.endTransmission();
    consensus();
    Serial.println(F("Mudou de referência"));
    Serial.print(F("Referência = "));
    Serial.println(ref);  
    
    refnow = refValue;
  }    
  
  if(feedValue == 1){
    ff = (ref - 1.389)/0.2123;
  }else{
    ff = 0;    
  }
  e = ref - lux_lido;

  if (e >= deadband)
    e = e - deadband;
  else if(e <= -deadband)
    e = e + deadband;
  else
    e = 0;
  
  p = K1 * ref - Kp * lux_lido;

  
  if (u_pwm_ant < 0 || u_pwm_ant > 255){
    u = p + intg;
    u_pwm = ((u-1.389)/0.2123) + ff;
  }else{
    intg = intg_ant + K2 * (e + e_ant); 
    u = p + intg;
    u_pwm = ((u-1.389)/0.2123) + ff;
    intg_ant = intg;
  }
  
  u_sat = constrain(u_pwm,0,255); //Saturates the u signal
  e_ant = e;
  u_pwm_ant = u_pwm;
  
  analogWrite(LED,u_sat);
    
  Serial.print(ref);
  Serial.print(F(" "));
  Serial.print(lux_lido);
  Serial.print(F(" "));
  Serial.print((u_sat/255)); // Percentagem entre 0 e 1 por causa das comunicaçoes
  Serial.print(F(" "));
  Serial.print(feedValue);
  Serial.print(F(" "));
  delta_t2 = micros();
  Serial.print((delta_t2-delta_t1)*0.001);
  Serial.println(F(";")); 
  delayMicroseconds(sample_time-(delta_t2-delta_t1));
  
} 
