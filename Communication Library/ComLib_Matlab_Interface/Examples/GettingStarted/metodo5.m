%% Cleanup and init
clc;
clear;
close all;

%% 1. Create radar system object
addpath('..\..\RadarSystem'); % aggiunyta MATLAB API
resetRS; % chiusura e cancellazione porte aperte in precedenza

szPort = findRSPort; % trova la porta COM corretta
if isempty(szPort)
    disp('No RadarSystem found.');
    return;
end
oRS = RadarSystem(szPort); % crea l'oggetto RadarSystem API 

%% 2. Imposta parametri radar
NTS = 256; % Number of samples per frame
Nm = 9;    % Number of frames per blocco
a = 10;    % Downsampling factor

oRS.oEPRadarS2GLP.parameters.number_of_samples = NTS;
oRS.oEPRadarS2GLP.parameters.frame_time_sec = 0.1500;
oRS.oEPRadarS2GLP.apply_parameters;

T = 1 * Nm; % periodo totale del blocco
N = (NTS * Nm) / a; % Numero di punti FFT dopo il downsampling

%% 3. Background
background = rand(1, N/2); % sostituire con Background reale

%% 4. inizializzazione dati per real-time plotting
hFig1 = figure('Name', 'Segnale Grezzo in Tempo Reale');
hAx1 = axes('Parent', hFig1);
hLine1 = plot(hAx1, nan(1, NTS * Nm)); % Placeholder for raw signal
xlabel(hAx1, 'Campioni');
ylabel(hAx1, 'Ampiezza');
title(hAx1, 'Segnale Grezzo Acquisito');

hFig2 = figure('Name', 'Segnale Downsampled in Tempo Reale');
hAx2 = axes('Parent', hFig2);
hLine2 = plot(hAx2, nan(1, N)); % Placeholder for downsampled signal
xlabel(hAx2, 'Campioni');
ylabel(hAx2, 'Ampiezza');
title(hAx2, 'Segnale Downsampled');

hFig3 = figure('Name', 'Spettro FFT in Tempo Reale');
hAx3 = axes('Parent', hFig3);
hLine3 = plot(hAx3, nan(1, N/2)); % Placeholder for FFT spectrum
xlabel(hAx3, 'Frequenza [Hz]');
ylabel(hAx3, 'Ampiezza');
title(hAx3, 'Spettro FFT');

%% 5. acquisizione e analisi dati
total_windows = 20; % Number of windows to process in real-time

for intervallo_tempo = 1:total_windows
    % Acquire frames from radar
    frames = [];
    for i = 1:Nm
        mxRawData = oRS.oEPRadarS2GLP.get_raw_data;
        frames = [frames; real(mxRawData.sample_data(:, 1))];
    end
    
    % Flatten and downsample frames
    block_frames = frames(:);
    block_frames = block_frames(1:a:end);
    
    % Perform FFT and remove background
    combined_fft = fft(block_frames, N);
    combined_fft_no_background = combined_fft(1:N/2) - background;
    xf = (0:(N/2)-1) * (1/T);
    
    % Update real-time plots
    set(hLine1, 'YData', frames(:)); % Update raw signal plot
    set(hLine2, 'YData', block_frames); % Update downsampled signal plot
    set(hLine3, 'YData', abs(combined_fft_no_background)); % Update FFT spectrum plot
    
    drawnow; % Refresh figures
end

%% 6. Clear radar system object
clearSP(szPort);
disp('Script completato con successo!');
