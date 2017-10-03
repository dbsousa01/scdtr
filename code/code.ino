
#define LED 13
#define LDR A0

float b = 1.808;
float m = -0.692;
int R1 = 10000;


float luximeter (int read_val){

  float F = (R1*(5.0-read_val*5.0/1023.0))/(read_val*5.0/1023.0);
  float lux_lido = pow(10,(log(F)-b)/m); // o erro é nesta linha, a potencia de 10 da muito pequena
  Serial.println(log(F));
  
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
  int AR = analogRead(LDR);
  float lux_lido;

  digitalWrite(LED, HIGH);
  delay(100);                       // wait for a second
  lux_lido= luximeter(AR);
  //Serial.println(lux_lido);
  //digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  //delay(1);                       // wait for a second
  //lux_lido= luximeter(AR);
  
  Serial.print(lux_lido);
  Serial.println(" lux");
  delay(100);        // delay in between reads for stability
}

