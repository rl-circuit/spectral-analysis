#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323

int main()
{
  double i;
  for(i = 0; i < 8000; i++) {
    printf("%f %f\n", i / 8000, sin(2*PI*i/8000));
  }
  return EXIT_SUCCESS;
}
