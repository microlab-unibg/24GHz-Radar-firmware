%% Cleanup
clc;
clear;
close all;

%% 1. Creazione dell'oggetto radar

% Aggiunta del percorso API Radar
addpath('..\..\RadarSystem');
resetRS;

% Inizializzazione radar
disp('Ricerca del Radar...');
szPort = findRSPort;
if isempty(szPort)
    disp('Radar non trovato.');
    return;
end
oRS = RadarSystem(szPort);

%% Parametri Radar
disp('Impostazione dei parametri radar...');
NTS = 256; % Numero di campioni per frame
Nm = 9; % Numero di frame per blocco
overlap = 4; % Sovrapposizione tra i blocchi
T = 1 * Nm; % Durata del blocco
N = NTS * Nm; % Numero totale di campioni per blocco
oRS.oEPRadarS2GLP.parameters.number_of_samples = NTS;
oRS.oEPRadarS2GLP.parameters.frame_time_sec = 0.1500;
oRS.oEPRadarS2GLP.parameters.min_speed_mps = 0.3;
oRS.oEPRadarS2GLP.apply_parameters;

%% 3. Background (simulazione)
background = rand(1, N/2); % Sostituire con un background reale 

%% 4. Acquisizione e analisi dei dati
% Variabili per memorizzare i massimi
max_amplitudes_BRPM = []; % Respirazione
max_amplitudes_BPM = []; % Battito

% Variabili per i segnali accumulati nel tempo
signal_I_total = []; % Segnale In-Phase totale
signal_Q_total = []; % Segnale Quadrature totale

% Figura per i grafici
hFig = figure('Name', 'FFT Frequenze', 'NumberTitle', 'off');
subplot(2, 2, 1); hTimeI = plot(nan); title('Segnale In-Phase (I)'); xlabel('Campioni'); ylabel('Ampiezza'); grid on;
subplot(2, 2, 2); hTimeQ = plot(nan); title('Segnale Quadrature (Q)'); xlabel('Campioni'); ylabel('Ampiezza'); grid on;
subplot(2, 2, [3, 4]); hFreq = plot(nan); title('Frequenze rilevate (0-30 Hz)'); xlabel('Frequenza [Hz]'); ylabel('Ampiezza'); grid on;

% Ciclo di acquisizione e analisi
disp('Inizio acquisizione e analisi...');
block_idx = 0;
step = Nm - overlap;
frame_buffer = [];

% Tempo massimo di acquisizione in secondi
tempo_massimo = 60; % 1 minuto
tempo_inizio = tic; % Inizia il timer

while ishandle(hFig) && toc(tempo_inizio) < tempo_massimo
    % Acquisizione dei dati grezzi
    raw_data = oRS.oEPRadarS2GLP.get_raw_data;
    frame_I = real(raw_data.sample_data(:)); % Segnale In-Phase (I)
    frame_Q = imag(raw_data.sample_data(:)); % Segnale Quadrature (Q)
    
    % Accumulo dei segnali nel tempo
    signal_I_total = [signal_I_total; frame_I];
    signal_Q_total = [signal_Q_total; frame_Q];
    
    frame_buffer = [frame_buffer; raw_data.sample_data(:)];
    
    % Aggiornamento dei grafici in tempo reale (dominio del tempo)
    set(hTimeI, 'YData', signal_I_total, 'XData', 1:length(signal_I_total));
    set(hTimeQ, 'YData', signal_Q_total, 'XData', 1:length(signal_Q_total));
    
    % Controllo se ci sono abbastanza frame per un blocco
    if length(frame_buffer) < Nm * NTS
        drawnow;
        continue;
    end

    % Estrazione di un blocco di dati
    block_frames = frame_buffer(1:Nm * NTS);
    frame_buffer(1:Nm * NTS) = []; % Rimuovi frame elaborati

    % Trasformata di Fourier
    combined_fft = fft(block_frames, N);
    combined_fft_no_background = combined_fft(1:N/2) - background;

    % Filtraggio delle frequenze
    xf = (0:(N/2)-1) * (1 / T);
    mask = (xf > 0 & xf <= 30);
    xf_filtered = xf(mask);
    fft_amplitudes_filtered = abs(combined_fft_no_background(mask));

    % Calcolo BRPM (respiro)
    resp_mask = (xf > 0 & xf <= 1);
    fft_resp = abs(combined_fft_no_background(resp_mask));
    max_amplitudes_BRPM = [max_amplitudes_BRPM, max(fft_resp)];

    % Calcolo BPM (battito)
    bpm_mask = (xf >= 0.66 & xf <= 6.66);
    fft_bpm = abs(combined_fft_no_background(bpm_mask));
    max_amplitudes_BPM = [max_amplitudes_BPM, max(fft_bpm)];

    % Aggiornamento del grafico delle frequenze
    set(hFreq, 'YData', fft_amplitudes_filtered, 'XData', xf_filtered);

    drawnow;

    % Avanza al prossimo blocco
    block_idx = block_idx + step;
end

%% Risultati finali
BRPM = mean(max_amplitudes_BRPM);
BPM = mean(max_amplitudes_BPM);

disp('Risultati:');
fprintf('BRPM: %.2f\n', (BRPM / 2) * 60);
fprintf('BPM: %.2f\n', (BPM / 2) * 60);

%% Salvataggio risultati
fileID = fopen('risultatiMATLAB.txt', 'w');
fprintf(fileID, 'BRPM medio: %.2f\n', (BRPM / 2) * 60);
fprintf(fileID, 'BPM medio: %.2f\n', (BPM / 2) * 60);
fclose(fileID);

disp('Analisi completata. Risultati salvati.');
