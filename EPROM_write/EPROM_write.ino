#include <EEPROM.h>

/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int addr = 0;
int val = 1;

void setup() {
  EEPROM.write(addr, val);
}

void loop() {}
  
  

