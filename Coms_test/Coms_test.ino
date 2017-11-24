
#include <Wire.h>
#include <EEPROM.h>

const int EPROM = 0;	//the other device address
const int add = EEPROM.read(EPROM);

char com_buf[5];
int buf_idx = 0;
float buf_num;

void receiveEvent(int howMany){
  while(Wire.available() > 0){
    char c_rec = Wire.read();	// receive byte as a character
    if (buf_idx == 3){
      com_buf[buf_idx] = c_rec;
      Serial.print("Buffer em chars: ");
      Serial.println(com_buf);    
      buf_num = atof(com_buf);
      Serial.print("Buffer em float: ");
      Serial.println(buf_num);
      Serial.println(buf_num * 2);
      buf_idx++;
    }else if(buf_idx == 4){buf_idx = 0;}
     else{
      com_buf[buf_idx] = c_rec;
      buf_idx++;
    }
    
  }
}

void setup(){
  Serial.begin(115200);
  Serial.println(add);
  Wire.begin(add);	 //join as a master/slave at the address saved in the EPROM
  Wire.onReceive(receiveEvent);	//register event handler
}

void loop(){
char c;
  if(Serial.available()>0){
    c = Serial.read();
    Wire.beginTransmission(2);	
    Wire.write(c);	
    Wire.endTransmission();
  }
}


