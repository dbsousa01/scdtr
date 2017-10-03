#define LED 13
#define LDR A0

float b = 1.808;
float m = -0.692;
int R1 = 10000;


float luximeter (int read_val){
  
  float F = (R1*(5-read_val*5/1023))/(read_val*5/1023);
  float lux_lido = pow(10,((log(F)-b)/m));
  
  return(lux_lido);
  delay(1);        // delay in between reads for stability
}


void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //pinMode(LED, OUTPUT);
  pinMode(LDR, INPUT);
  
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int AR = analogRead(LDR);
  float lux_lido = luximeter(AR);
  
  Serial.println(lux_lido);
  delay(1);        // delay in between reads for stability
}

