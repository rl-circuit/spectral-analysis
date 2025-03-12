#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979323


typedef struct {
  double real;
  double imm;
} complex_t;


typedef struct {
  double t;
  double y; /* y(t) */
} data_t;


void arg(complex_t* n, double* argument)
{
  if(n->real == 0) {
    if(n->imm == 0)
      *argument = 0;
    else if(n->imm < 0)
      *argument = PI * 1.5;
    else
      *argument = PI * 0.5;
  }
  else if(n->real > 0)
    *argument = atan(n->imm / n->real);
  else
    *argument = atan(n->imm / n->real) + PI;
}


void modulus(complex_t* n, double* mod)
{
  *mod = sqrt(pow(n->real, 2) + pow(n->imm, 2));
}


void add(complex_t* n1, complex_t* n2, complex_t* result)
{
  result->real = n1->real + n2->real;
  result->imm = n1->imm + n2->imm;
}


void conjugate(complex_t* n)
{
  n->imm = -(n->imm);
}


void dot(complex_t* n1, complex_t* n2, complex_t* result)
{
  double argn1, modn1;
  double argn2, modn2;
  double argResult, modResult;

  arg(n1, &argn1);
  modulus(n1, &modn1);
  arg(n2, &argn2);
  modulus(n2, &modn2);

  modResult = modn1 * modn2;
  argResult = argn1 + argn2;
  
  result->real = modResult * cos(argResult);
  result->imm = modResult * sin(argResult);
}


void power(complex_t* n, double a)
{
  double modn, argn;

  arg(n, &argn);
  modulus(n, &modn);
  n->real = modn * cos(a * argn);
  n->imm = modn * sin(a * argn);
}


void dft(data_t *a, complex_t *b, int N, int samplingFreq, FILE *r)
{
  int n, k;
  complex_t point, coefficient, product;
  double magnitude, frequency_resolution, f = 0;
  
  frequency_resolution = (double)samplingFreq / N;
  
  for(k = 0; k < (int)(samplingFreq / (frequency_resolution * 2)); k++) {
    for(n = 0; n < N; n++) {
      point.real = a[n].y;
      point.imm = 0;
      
      coefficient.real = cos(-2*PI*k*n / N);
      coefficient.imm = sin(-2*PI*k*n / N);
      
      dot(&point, &coefficient, &product);
      
      b[k].real += product.real;
      b[k].imm += product.imm;
    }
    modulus(&b[k], &magnitude);
    fprintf(r, "%f %f\n", f, magnitude * 2.0 / N);
    f += frequency_resolution;
  }
}

int main(int argc, char** argv)
{
  FILE *rawData, *results;
  data_t *read;
  complex_t *bins;
  int k, N_samples = 0, samplingFreq;
  char ch;

  if(argc != 3) {
    printf("Utilizzo: %s <raw_txt_data> <results_txt_name>\n", argv[0]);
    return EXIT_FAILURE;
  }

  rawData = fopen(argv[1], "r");

  if(rawData == NULL) {
    printf("Impossibile aprire il file di campionamento!\n");
    fclose(rawData);
    return EXIT_FAILURE;
  }

  results = fopen(argv[2], "w");

  if(results == NULL) {
    printf("Impossibile aprire il file dei risultati!\n");
    fclose(results);
    return EXIT_FAILURE;
  }

  while ((ch = fgetc (rawData)) != EOF) {
    if (ch == '\n') {
      N_samples++;
    }
  }

  printf("N samples: %d\n", N_samples);

  rewind(rawData);
  
  read = malloc(sizeof(data_t) * N_samples);

  k = 0;

  while (!feof(rawData)) {
    fscanf(rawData, "%lf %lf", &read[k].t, &read[k].y);
    if (feof(rawData)) break; // Break the loop when reaching EOF
    k++;
  }
  
  printf("Enter the sampling frequency (Hz): ");
  scanf("%d", &samplingFreq);
  
  bins = malloc(sizeof(complex_t) * samplingFreq);
  for(k = 0; k < samplingFreq; k++) {
    bins[k].real = 0;
    bins[k].imm = 0;
  }
  
  dft(read, bins, N_samples, samplingFreq, results);
  
  free(read);
  free(bins);
  fclose(rawData);
  fclose(results);
  return EXIT_SUCCESS;
}
