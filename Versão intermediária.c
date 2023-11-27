#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kiss_fft.h"

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

#define N 16

// Função para calcular a Distorção Harmônica Total (THD)
double calculate_thd(const kiss_fft_cpx *spectrum, size_t size) {
    // Assume que o primeiro bin (índice 0) representa a componente fundamental
    double fundamental_amplitude = sqrt(spectrum[0].r * spectrum[0].r + spectrum[0].i * spectrum[0].i);

    // Calcula a soma dos quadrados das amplitudes das componentes harmônicas
    double harmonic_sum = 0.0;
    size_t i;
    for (i = 1; i < size; i++) {
        harmonic_sum += spectrum[i].r * spectrum[i].r + spectrum[i].i * spectrum[i].i;
    }

    // Calcula a THD
    double thd = sqrt(harmonic_sum) / fundamental_amplitude;

    return thd;
}


void print_complex_array(const char *label, const kiss_fft_cpx *arr, int size) {
    printf("%s\n", label);
    int i;
    for (i = 0; i < size; i++) {
        printf("arr[%d] = %f , %f\n", i, arr[i].r, arr[i].i);
    }
}
void save_complex_array(const char *filename, const kiss_fft_cpx *arr, size_t size) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo");
        return;
    }

    size_t i;
    for (i = 0; i < size; i++) {
        fprintf(file, "%.3f %.3f\n", arr[i].r, arr[i].i);
    }

    fclose(file);
}
void TestFft(const char* title, const kiss_fft_cpx in[N], kiss_fft_cpx out[N])
{
  kiss_fft_cfg cfg;

  printf("%s\n", title);

  if ((cfg = kiss_fft_alloc(N, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
  {
    kiss_fft(cfg, in, out);

    // Imprime o array de entrada e o array de saída
    //print_complex_array("Entrada (complexa):", in, N);
    //print_complex_array("Saída (complexa):", out, N);

    // Salva os arrays de entrada e saída em arquivos
    //save_complex_array("entrada.txt", in, N);
   // save_complex_array("saida.txt", out, N);

    // Libera a memória alocada para a configuração
    free(cfg);
  }
  else
  {
    printf("not enough memory?\n");
    exit(-1);
  }
}

int main(void)
{
  kiss_fft_cpx in[N], out[N];
  size_t i;

  for (i = 0; i < N; i++)
    in[i].r = sin(2 * M_PI * 4 * i / N), in[i].i = 0;
  TestFft("SineWave (complex)", in, out);

  // Calcula e imprime a THD
  double thd = calculate_thd(out, N);
  printf("THD: %f\n", thd);

  return 0;
}
