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
N = floor((NTS * Nm) / a); % Assicura che N sia un numero intero

oRS.oEPRadarS2GLP.parameters.number_of_samples = NTS;
oRS.oEPRadarS2GLP.parameters.frame_time_sec = 0.1500;
oRS.oEPRadarS2GLP.apply_parameters;

%% 3. Acquisizione del background senza persona presente
disp('Acquisizione del background, assicurarsi che nessuna persona sia davanti al radar...');
pause(3); % Attendere per assicurarsi che l'utente sia pronto

num_background_frames = 20; % Numero di frame per il background
background_frames = [];
for i = 1:num_background_frames
    mxRawData = oRS.oEPRadarS2GLP.get_raw_data;
    background_frames = [background_frames; real(mxRawData.sample_data(:,1))];
end

background_signal = mean(background_frames, 1); % Media dei frame per ottenere il background
background_fft = abs(fft(background_signal, N));
background = background_fft(1:N/2); % Prendere solo la metà positiva dello spettro

disp('Background acquisito. Ora posizionarsi davanti al radar.');
pause(5); % Attendere il posizionamento della persona

%% 4. Acquisizione e analisi dei dati in tempo reale
max_amplitudes_BRPM = [];
max_amplitudes_BPM = [];
total_windows = 10; % Numero totale di finestre da analizzare

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
    combined_fft = abs(fft(block_frames, N));
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
    
    % Calcolo e stampa in tempo reale
    BRPM = (mean(max_amplitudes_BRPM) / 2) * 60;
    BPM = mean(max_amplitudes_BPM) * 60;
    
    clc;
    fprintf('Intervallo: %d\n', intervallo_tempo);
    fprintf('Respiro al minuto (BRPM): %.2f\n', BRPM);
    fprintf('Battiti al minuto (BPM): %.2f\n', BPM);
    pause(1); % Attesa per simulare il tempo reale
end

%% 5. Chiusura e pulizia
clearSP(szPort);
disp('Script completato con successo!');