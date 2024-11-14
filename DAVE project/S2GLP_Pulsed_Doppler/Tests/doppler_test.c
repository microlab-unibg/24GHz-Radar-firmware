
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "doppler.h"

#define TEST_NUM_SAMPLES 256
#define TEST_ADC_FREQ 1000 // Frequenza di campionamento in Hz
#define FILE_NAME "test_data.txt"

// Funzione per generare dati randomici
void generate_random_data(float *i_data, float *q_data, int num_samples) {
    srand(time(NULL)); // Imposta il seed per la generazione casuale
    for (int i = 0; i < num_samples; i++) {
        i_data[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // Dato randomico tra -1.0 e 1.0
        q_data[i] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
    }
}

// Funzione per scrivere i dati su un file
void write_data_to_file(const char *filename, float *i_data, float *q_data, int num_samples) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Errore nell'aprire il file!\n");
        exit(1);
    }

    for (int i = 0; i < num_samples; i++) {
        fprintf(file, "%f %f\n", i_data[i], q_data[i]); // Scrive i dati I e Q nel file
    }

    fclose(file);
    printf("Dati scritti nel file %s\n", filename);
}

// Funzione per leggere i dati da un file
void read_data_from_file(const char *filename, float *i_data, float *q_data, int num_samples) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Errore nell'aprire il file per la lettura!\n");
        exit(1);
    }

    for (int i = 0; i < num_samples; i++) {
        fscanf(file, "%f %f", &i_data[i], &q_data[i]); // Legge i dati I e Q dal file
    }

    fclose(file);
    printf("Dati letti dal file %s\n", filename);
}

// Funzione di test
void test_doppler_algorithm() {
    // Buffer dei dati simulati
    float raw_data_i[TEST_NUM_SAMPLES];
    float raw_data_q[TEST_NUM_SAMPLES];

    // Genera dati randomici
    generate_random_data(raw_data_i, raw_data_q, TEST_NUM_SAMPLES);

    // Scrive i dati generati su un file
    write_data_to_file(FILE_NAME, raw_data_i, raw_data_q, TEST_NUM_SAMPLES);

    // Inizializza buffer vuoti per leggere i dati dal file
    float file_data_i[TEST_NUM_SAMPLES];
    float file_data_q[TEST_NUM_SAMPLES];

    // Legge i dati dal file
    read_data_from_file(FILE_NAME, file_data_i, file_data_q, TEST_NUM_SAMPLES);

    // Struct
    algo_settings_t algo_settings = {
        .min_speed_kmph = 1.0f,  // Velocità minima in km/h
        .max_speed_kmph = 10.0f, // Velocità massima in km/h
        .speed_detection_threshold = 0.5f,
        .motion_detection_threshold = 0.2f,
        .use_high_gain_doppler = 1
    };

    device_settings_t dev_settings = {
        .adc_sampling_freq_Hz = TEST_ADC_FREQ
    };

    algo_result_t algo_result;
    memset(&algo_result, 0, sizeof(algo_result)); // Inizializza la struttura del risultato

    // Configurazione della finestra FFT
    FFT_Window_Struct_t fft_window = {
        .fft_size = TEST_NUM_SAMPLES
    };

    // Esegui il calcolo del doppler sui dati letti dal file
    doppler_calc_speed(fft_window, file_data_i, file_data_q, TEST_NUM_SAMPLES, &algo_settings, &dev_settings, &algo_result);

    // Verifica dei risultati
    printf("Risultati dell'algoritmo Doppler:\n");
    printf("Frequenza Doppler rilevata (Hz): %f\n", algo_result.doppler_frequency_hz);
    printf("Velocità rilevata (km/h): %f\n", algo_result.velocity_kmph);
    printf("Livello del segnale: %f\n", algo_result.level);
    printf("Target in avvicinamento: %d\n", algo_result.target_approaching);
    printf("Target in allontanamento: %d\n", algo_result.target_departing);
    printf("Rilevato movimento: %d\n", algo_result.motion_detected);
}

int main() {
    test_doppler_calc_speed();
    return 0;
}




