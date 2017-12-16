
#include <Wire.h>
#include <EEPROM.h>


const int EPROM = 0;	
const int self_add = EEPROM.read(EPROM);
int send_add = -self_add + 3;

float buf_num;

float b;
float m = -0.692;
int R1 = 10;

float dii=0 , dij=0, djj=0 , dji=0 ;

//..................................WIRE RECEIVE.......................................//
void receiveEvent(int howMany){
  char c_rec;
<<<<<<< HEAD
  char com_buf[9] = {};
=======
  char com_buf[10];
>>>>>>> 3171c64e07d5ea9ca2e9fb3fab486b3ab68dbb94
  char * strtokaux1;
  int buf_idx=0;
  while(Wire.available() > 0){
    c_rec = Wire.read();	// receive byte as a character 
<<<<<<< HEAD
    if( c_rec != '\0' && c_rec != '\n' ){
      com_buf[buf_idx] = c_rec;
      buf_idx++;
    }
  }
  Serial.println(buf_idx);
=======
    com_buf[buf_idx] = c_rec;
    buf_idx++;
  }
  
>>>>>>> 3171c64e07d5ea9ca2e9fb3fab486b3ab68dbb94
  Serial.println(com_buf);
  
  strtokaux1 = strtok(com_buf, ";");
  djj = atof(strtokaux1);

  strtokaux1 = strtok(NULL, ";");
  dji = atof(strtokaux1);
  
  Serial.print("Valor de djj recebido: ");
  Serial.println(djj);
  Serial.print("Valor de dji recebido: ");
  Serial.println(dji);
}
//....................................................................................//


void setup(){
  Serial.begin(115200);
  
  Serial.println("Beginning of the program");
  Serial.print("Address of this luminaire: ");
  Serial.println(self_add);
  delay(1000);  // allow things to stabilize
  Wire.begin(self_add);	 //join as a master/slave at the address saved in the EPROM
  Wire.onReceive(receiveEvent);	//register event handler

}

void loop(){

<<<<<<< HEAD
  float value1 = 7.23;
  float value2 = 0.49;
=======
  float value1 = 8.00;
  float value2 = 1.56;
>>>>>>> 3171c64e07d5ea9ca2e9fb3fab486b3ab68dbb94
  char b1[4+1];
  char b2[4+1];
  
  char b[10];
  
  dtostrf(value1,3,2,b1);
  dtostrf(value2,3,2,b2);
  
<<<<<<< HEAD
  sprintf(b,"%s;%s\0",b1,b2);
=======
  sprintf(b,"%s;%s",b1,b2);
>>>>>>> 3171c64e07d5ea9ca2e9fb3fab486b3ab68dbb94
  
  Wire.beginTransmission(send_add);
  Wire.write(b);
  Wire.endTransmission();
  delay(1000);

}


