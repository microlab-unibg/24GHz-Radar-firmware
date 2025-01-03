%% Cleanup e inizializzazione
clc;
clear;
close all;

%% 1. Creazione dell'oggetto radar
addpath('..\..\RadarSystem'); % Aggiungi API di MATLAB
resetRS; % Chiude e cancella le porte aperte precedentemente

szPort = findRSPort; % Trova la porta COM corretta
if isempty(szPort)
    disp('Nessun sistema radar trovato.');
    return;
end

oRS = RadarSystem(szPort); % Crea l'oggetto API del sistema radar

%% 2. Imposta i parametri del radar
NTS = 256; % Numero di campioni per frame
Nm = 9;    % Numero di frame per blocco
a = 10;    % Fattore di downsampling

T = 1 * Nm; % Periodo totale del blocco
N = (NTS * Nm) / a; % Numero di punti FFT dopo il downsampling

oRS.oEPRadarS2GLP.parameters.number_of_samples = NTS;
oRS.oEPRadarS2GLP.parameters.frame_time_sec = 0.1500;
oRS.oEPRadarS2GLP.apply_parameters;

%% 3. Acquisizione del background (simulazione)
background = rand(1, N/2); % Sostituire con un background reale se disponibile

%% 4. Acquisizione e analisi dei dati
max_amplitudes_BRPM = [];
max_amplitudes_BPM = [];
total_windows = 10; % Numero totale di finestre da analizzare


pdf_filename = 'grafici_outputM4.pdf';
results_filename = 'risultatiM4.txt';
pdfObj = matlab.graphics.internal.createPDF(pdf_filename);

for intervallo_tempo = 1:total_windows
    % Acquisizione dei frame dal radar
    frames = [];
    for i = 1:Nm
        mxRawData = oRS.oEPRadarS2GLP.get_raw_data;
        frames = [frames; real(mxRawData.sample_data(:,1))];
    end
    
    % Flatten e downsampling
    block_frames = frames(:);
    block_frames = block_frames(1:a:end);
    
    % FFT e rimozione del background
    combined_fft = fft(block_frames, N);
    combined_fft_no_background = combined_fft(1:N/2) - background;
    xf = (0:(N/2)-1) * (1/T);
    
    % Selezione delle ampiezze per il respiro (0-1 Hz)
    mask_resp = (xf > 0) & (xf <= 1);
    fft_amplitudes_selected = combined_fft_no_background(mask_resp);
    [~, max_idx_resp] = max(abs(fft_amplitudes_selected));
    max_amplitudes_BRPM(end+1) = abs(fft_amplitudes_selected(max_idx_resp));
    
    % Selezione delle ampiezze per il battito (0.66-6.66 Hz)
    mask_battito = (xf > 0.66) & (xf <= 6.66);
    fft_amplitudes_selected2 = combined_fft_no_background(mask_battito);
    [~, max_idx_battito] = max(abs(fft_amplitudes_selected2));
    max_amplitudes_BPM(end+1) = abs(fft_amplitudes_selected2(max_idx_battito));
    
    % Plot dei dati e salvataggio nel PDF
    figure;
    plot(xf, abs(combined_fft_no_background));
    title(['Intervallo di tempo ', num2str(intervallo_tempo)]);
    xlabel('Frequenza [Hz]');
    ylabel('Ampiezza');
    grid on;
   
end

%% 5. Calcolo dei risultati finali
BRPM = mean(max_amplitudes_BRPM);
BPM = mean(max_amplitudes_BPM);

fileID = fopen(results_filename, 'w');
fprintf(fileID, 'Media dei massimi valori di ampiezza tra 0 e 1 Hz: %.2f\n', BRPM);
fprintf(fileID, 'BRPM: %.2f\n', (BRPM / 2) * 60);
fprintf(fileID, 'Media dei massimi valori di ampiezza tra 0.66 e 6.66 Hz: %.2f\n', BPM);
fprintf(fileID, 'BPM: %.2f\n', BPM * 60);
fclose(fileID);

%% 6. Chiusura e pulizia
clearSP(szPort);
close(pdfObj);
disp('Script completato con successo!');
