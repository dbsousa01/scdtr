#include <math.h>

float b = 1.808;
float m = -0.692;
int R1 = 10;

double luximeter (double read_val){

  double F = (R1*(5.0-read_val*5.0/1023.0))/(read_val*5.0/1023.0);
  double lux_lido = pow(10,(log10(F)-b)/m);
    
  return(lux_lido);
}
