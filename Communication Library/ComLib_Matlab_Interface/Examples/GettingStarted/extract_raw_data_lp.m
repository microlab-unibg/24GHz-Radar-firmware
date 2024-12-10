%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function out = extract_raw_data (in)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Copyright (c) 2014-2021, Infineon Technologies AG
% All rights reserved.
%
% Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
% following conditions are met:
%
% Redistributions of source code must retain the above copyright notice, this list of conditions and the following
% disclaimer.
%
% Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
% disclaimer in the documentation and/or other materials provided with the distribution.
%
% Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
% products derived from this software without specific prior written permission.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
% INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
% DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
% SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
% SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
% WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
% OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DESCRIPTION:
% This script showcases the acquisition and live visualization of raw data.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Cleanup and init
% Before starting any kind of device the workspace must be cleared and the
% MATLAB Interface must be included into the code.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
clear;
close all;

%% 1. Create radar system object
disp('******************************************************************');
addpath('..\..\RadarSystem'); % add MATLAB API
resetRS; % close and delete ports

szPort = findRSPort; % find the right COM Port
if (isempty(szPort))
    % try 2nd time
    szPort = findRSPort; % find the right COM Port
end
if (isempty(szPort))
    disp ('No RadarSystem found.');
    return;
end

oRS = RadarSystem(szPort); % create RadarSystem API object

%% 2. Display device information
board_info = oRS.oEPRadarBaseBoard.get_board_info;
shield_info = oRS.oEPRadarS2GLP.get_shield_info;
current_info = oRS.oEPRadarBaseBoard.consumption;
current_def = oRS.oEPRadarBaseBoard.consumption_def;

disp('Connected RadarSystem:');
disp(board_info.description);
disp(shield_info.description);
fprintf('%s: %f %s\n\n',current_def, current_info.value, current_info.unit);

%% 3. Set radar parameters
%%% Display default parameters
disp('Default Radar Parameters:');
oRS.oEPRadarBaseBoard.reset_parameters; % reset radar parameters
oRS.oEPRadarS2GLP.get_parameters_def % display default radar parameters

%%% Change parameters in memory
NTS = 256;
%oRS.oEPRadarS2GLP.parameters.number_of_samples = NTS;
%oRS.oEPRadarS2GLP.parameters.frame_time_sec = 0.1500;
oRS.oEPRadarS2GLP.parameters.frame_time_sec = 2.0; %aumento tempo frame per migliorare risoluzione
%oRS.oEPRadarS2GLP.parameters.min_speed_mps = 0.3;
oRS.oEPRadarS2GLP.parameters.min_speed_mps = 1.9; %velocità minima
oRS.oEPRadarS2GLP.parameters.doppler_sensitivity = 2000; %aumento sensibilità doppler
oRS.oEPRadarS2GLP.parameters.number_of_samples = 256 %numero campioni maggiore per risoluzione temporale migliore

%%% Send parameters to device
oRS.oEPRadarS2GLP.apply_parameters;

%%% Get and display set parameters
disp('Set Radar Parameters:');
param_set = oRS.oEPRadarS2GLP.get_parameters

%% 4. Collect data
%%% List of get-commands
% oRS.oEPRadarS2GLP.get_result_data
% oRS.oEPRadarS2GLP.get_raw_data
% oRS.oEPRadarS2GLP.get_result_and_raw_data
% oRS.oEPRadarBaseBoard.get_consumption

%%% Initialize figure plotting first frame
disp('Plot raw data...');
hFig = figure;

%applico un filtro passa-basso
freq_s = 5000;  %frequenza campionamento
freq_c =  1;    %freq di taglio 
[b, a] = butter(4, freq_c/(freq_s/2), 'low'); %filtro Butterwoeth

%mxRawData = oRS.oEPRadarS2GLP.get_raw_data;
%fprintf('Frame number: %d\n', mxRawData.frame_number);

%plot_data = [real(mxRawData.sample_data(:,1)), imag(mxRawData.sample_data(:,1))];
%hData = plot(plot_data);
%xlim([1,NTS]);
%ylim([0,1]);
%xlabel('Sample');
%ylabel('ADC Value (FSR)');
%title('Raw Data');
%legend(['I';'Q']);

%drawnow;

%%% Start infinite loop to get and plot raw data
while ishandle(hFig)
    
    %acquisizione dati grezzi
    mxRawData = oRS.oEPRadarS2GLP.get_raw_data;
    fprintf('Frame number: %d\n',mxRawData.frame_number);

    %estrazione dati I e Q
    raw_I = real(mxRawData.sample_data(:,1));
    raw_Q = imag(mxRawData.sample_data(:,1));

    %filtro passa-basso sui dati grezzi
    filtered_I = filtfilt(b, a, raw_I);
    filtered_Q = filtfilt(b, a, raw_Q);

    %calcolo FFT
    N = length(filtered_I);%num campioni
    f = (0:N-1)*(freq_s/N);%asse delle freq
    Y = abs(fft(filtered_I));%magnitudine fft

    %plot dati filtrati
    subplot(2,1,1);
    plot(filtered_I);
    hold on;
    plot(filtered_Q);
    xlabel('Campioni');
    ylabel('Ampiezza');
    title('Segnale Filtrato (I e Q)');
    legend('In-Phase (I)', 'Quadratura (Q)');

    % Plot dell'analisi in frequenza (FFT)
    subplot(2,1,2);
    plot(f, Y);
    xlim([0 5]); % Frequenze basse per rilevare respiro e battito (0-5 Hz)
    xlabel('Frequenza [Hz]');
    ylabel('Ampiezza');
    title('Analisi in Frequenza del Segnale');
    
    %plot_data = [real(mxRawData.sample_data(:,1)), imag(mxRawData.sample_data(:,1))];
    %plot_data_cell = mat2cell(transpose(plot_data),ones(1,2));
    %set(hData,{'YData'},plot_data_cell);
    
    drawnow;
    
end

%% 5. Clear radar system object
disp('Clear radar object...');
clearSP(szPort);

%% 6. End of script
disp('Done!');

