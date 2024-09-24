#include "doppler.h"
#include <stdio.h>
#include <stdint.h>

#define NUM_SAMPLES //???

void read_data_file(const char*data_test, uint16_t num_samples, float* data_i[], float* data_q[]){

    FILE *file = fopen(data_test, "r");
    if(file == NULL){
        printf("Errore apertura file\n");
        return;
    }

    for(int i; i < num_samples; i++){
        fscanf(file, "%f, %f", &data_i[i], &data_q[i]);
    }

    fclose(file);
}

int main(){
    //creo array dati
    float data_i[NUM_SAMPLES];
    float data_q[NUM_SAMPLES];

    //leggo dati 
    read_data_file("test_datastore", NUM_SAMPLES, data_i, data_q);

    //imposto valori settaggi input e out
    algo_settings_t algo_setting = {.min_speed_kmph = 0, .max_speed_kmph= 0};
    device_settings_t device_setting = {};
    
    //creo struttura per risultati
    algo_result_t struct_result;

    //creo fft (HO PRESO IL CODICE DI DOPPLER.C)

    static FFT_Window_Struct_t fft_window =
    {
        .fft_window_type = FFT_WINDOW_CHEBYSHEV,       /* Type of window defined by \ref FFT_Window_Type_t */
        .fft_size = FFT_SIZE_256,                      /* FFT size \ref FFT_Size_t  */
        .fft_window_length = 0,                        /* Number of elements in the FFT window  */
        //TODO: To be checked with CW the value of AT
        .fft_chebyshev_at_dB = 60,                     /* Attenuation parameter needed in case of Chebyshev window */
        .fft_time_twiddle_table = NULL,                /* Pointer to twiddle factor table for FFT windowing over time */
        .fft_window_buffer = fft_fast_window_buffer    /* Pointer to floating point memory containing data values for FFT windowing over time */
    };

   

    //chiamo funzione 
    doppler_calc_speed(fft_window, data_i, data_q, NUM_SAMPLES, algo_setting, device_setting, struct_result)

    //stampo valori da controllare
    printf("Frequenza = ", struct_result.doppler_frequency_hz, "\n");
    printf("Velocità = ", struct_result.velocity_kmph, "\n");

    return 0;
}


