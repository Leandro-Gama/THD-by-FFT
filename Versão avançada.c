#include <stdio.h>
#include <math.h>
#include "kiss_fft.h"

#ifndef M_PI
#define M_PI 3.14159265358979324
#endif

#define N 1000

void generate_multiharmonic_signal(kiss_fft_cpx *signal, size_t size) {
    double t;
    for (size_t i = 0; i < size; i++) {
        t = i * 0.001;
        signal[i].r = 2 * cos(2 * M_PI * 100 * t) + 0.01 * cos(2 * M_PI * 200 * t) +
                       0.005 * cos(2 * M_PI * 300 * t);
        signal[i].i = 0.0;
    }
}

void save_complex_array(const char *filename, const kiss_fft_cpx *arr, size_t size) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    for (size_t i = 0; i < size; i++) {
        fprintf(file, "%f %f\n", arr[i].r, arr[i].i);
    }

    fclose(file);
}

void save_thd_components(const char *filename, const kiss_fft_cpx *spectrum, size_t size, size_t fundamental_index) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    fprintf(file, "Componentes harmônicas que contribuem para a THD:\n");

    for (size_t i = 1; i < size; i++) {
        if (i != fundamental_index) {
            fprintf(file, "Harmônica %zu: %+.12f , %+.12f\n", i, spectrum[i].r, spectrum[i].i);
        }
    }

    fclose(file);
}

// Função para calcular a Distorção Harmônica Total (THD)
double calculate_thd(const kiss_fft_cpx *spectrum, size_t size) {
    // Encontra a componente com a maior amplitude (não inclui o DC)
    size_t fundamental_index = 1;  // Começa do segundo bin
    double max_amplitude = 0.0;

    for (size_t i = 1; i < size; i++) {
        double amplitude = sqrt(spectrum[i].r * spectrum[i].r + spectrum[i].i * spectrum[i].i);
        if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
            fundamental_index = i;
        }
    }

    // Salva os arrays de entrada e saída em arquivos
    save_complex_array("entrada.txt", spectrum, size);
    save_complex_array("saidafft.txt", spectrum, size);

    // Salva as componentes harmônicas que contribuem para a THD
    save_thd_components("harmonicas.txt", spectrum, size, fundamental_index);

    // Calcula a soma dos quadrados das amplitudes das outras componentes
    double harmonic_sum = 0.0;
    for (size_t i = 1; i < size; i++) {
        if (i != fundamental_index) {
            harmonic_sum += spectrum[i].r * spectrum[i].r + spectrum[i].i * spectrum[i].i;
        }
    }

    // Calcula a THD
    double thd = sqrt(harmonic_sum) / max_amplitude;

    return thd;
}

int main(void) {
    kiss_fft_cpx in[N], out[N];
    kiss_fft_cfg cfg;

    // Gera um sinal com a senoide fornecida como entrada
    generate_multiharmonic_signal(in, N);

    // Inicializa a configuração da FFT
    cfg = kiss_fft_alloc(N, 0, 0, 0);

    // Realiza a FFT
    kiss_fft(cfg, in, out);

    // Calcula e salva os resultados em arquivos
    double thd = calculate_thd(out, N);

    printf("THD: %f\n", thd);

    // Libera a memória alocada para a configuração
    free(cfg);

    // Aguarda a entrada do usuário antes de encerrar
    getchar();

    return 0;
}
