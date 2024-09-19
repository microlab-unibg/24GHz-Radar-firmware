%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
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
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% DESCRIPTION:
%
% This examples shows the Doppler processing for the collected raw
% data and computation of the target.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% NOTES:
%
% For Doppler modulation there is no FMCWEndpoint in the XML file, the
% range FFT has to be omitted and the Doppler FFT has to be directly computed.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Startup
clc;
clear;
close all;

%% Constants & Parameters
c0 = 3e8;                   % Speed of light in vaccuum [m/s]
zero_padding_factor = 2;

%% Raw Data Name
fdata = 'data_S2GLP';

%% XML2STRUCT
%  to parse the XML file, the package XML2STRUCT is requried.
%  Please download the package from
%  https://de.mathworks.com/matlabcentral/fileexchange/28518-xml2struct
%  unzip it and copy the files into this folder
%  the function f_parse_data is not compatible with the build-in matlab
%  function!
%
if not(isfile("xml2struct.m"))
   error("Please install xml2struct.m, please see comments in the source file above!") 
end

%% Load Raw Data & Radar Settings
[frame, frame_count, results, sXML, Header] = f_parse_data_s2glp(fdata); % Data Parser

% Frame duration
frame_period = Header.Frame_period_sec;

num_Tx_antennas = Header.Num_Tx_Antennas; % Number of Tx antennas
num_Rx_antennas = Header.Num_Rx_Antennas; % Number of Rx antennas

% Number of ADC samples per chirp
NTS = Header.Samples_per_Chirp;

% Number of chirps per frame
PN = Header.Chirps_per_Frame;

% Sampling frequency
fS = Header.Sampling_Frequency_kHz * 1e3;

% RF frequency
fC = str2double(sXML.Device.TjpuEndpoint.DeviceInfo.maxRfFrequency_kHz.Text)*1e3;

% Doppler threshold
doppler_threshold = str2double(sXML.Device.TjpuEndpoint.TJPUPulseParaValues.doppler_sensitivity_nu.Text);

% Motion threshold
motion_threshold = str2double(sXML.Device.TjpuEndpoint.TJPUPulseParaValues.motion_sensitivity_nu.Text);

%% Calculate Derived Parameters
lambda = c0 / fC;                               % wavelength

doppler_fft_size = NTS * zero_padding_factor;
doppler_window_func = chebwin(NTS,60);          % Window function for Doppler

doppler_Hz_per_bin =  fS / doppler_fft_size;    
Hz_to_mps_constant = lambda / 2;                % Conversion factor from frequency to speed in m/s
IF_scale = 8 * 3.3;                             % Scaling factor for signal strength

fD_max = fS / 2;                                % Maximum theoretical value of the Doppler frequency
fD_per_bin = fD_max / (doppler_fft_size/2);     % Doppler bin size in Hz
array_bin_fD = ((1:doppler_fft_size) - doppler_fft_size/2 - 1) * -fD_per_bin * Hz_to_mps_constant; % Vector of speed in m/s

%% Initialize Debug Structures
doppler_data_complete             = zeros(doppler_fft_size, frame_count);
doppler_level_complete            = zeros(1, frame_count);
doppler_frequency_complete        = zeros(1, frame_count);
doppler_velocity_complete         = zeros(1, frame_count);
doppler_motion_direction_complete = zeros(1, frame_count);

%% Process Frames
for fr_idx = 1:frame_count                      % Loop over all data frames, while the output window is still open
    raw_data = frame(fr_idx).Chirp;             % Raw data for the frame being processed
    raw_data = raw_data(:,1,1);                 % Select high gain output data
    
    raw_data = raw_data * IF_scale;
    raw_data = raw_data - mean(raw_data);
    
    raw_data_windowed = raw_data .* doppler_window_func;
    doppler_data = fftshift(fft(raw_data_windowed,doppler_fft_size)); % Windowing across range and range FFT
    
    doppler_spectrum = abs(doppler_data);
    doppler_spectrum = doppler_spectrum(:,1);
    doppler_spectrum(1) = 0;                    % Ignore maximum speed since the sign of velocity cannot be determined
    
    [doppler_level, max_idx] = max(doppler_spectrum);
    fprintf('Frame number: %d',fr_idx);
    
    doppler_frequency = 0;
    doppler_speed = 0;
    doppler_velocity = 0;
    motion_dir = 0;
    
    if doppler_level > doppler_threshold
        doppler_frequency =  max_idx * doppler_Hz_per_bin;
        doppler_velocity  = array_bin_fD(max_idx);
        
        if (doppler_velocity < 0 )                %Direction of motion detection
            fprintf(', Target departing: %.2f m/s\n',abs(doppler_velocity));
            motion_dir = 1;
        else
            fprintf(', Target approaching: %.2f m/s\n',doppler_velocity);
            motion_dir = -1;
        end
    elseif doppler_level > motion_threshold
        fprintf(', Motion detected\n');
    else
        fprintf('\n');
    end
    
    doppler_data_complete(:,fr_idx) = doppler_data;
    doppler_level_complete(fr_idx) = doppler_level;
    doppler_frequency_complete(fr_idx) = doppler_frequency;
    doppler_velocity_complete(fr_idx) = doppler_velocity;
    doppler_motion_direction_complete(fr_idx) = motion_dir;
end

%% Visualization
results_velocity_mps = cell2mat({results.data.velocity_mps});

figure;

subplot(2,1,1);
plot(1:frame_count,doppler_velocity_complete);
hold on;
plot(1:frame_count,results_velocity_mps);
xlabel('Frame');
ylabel('Velocity (m/s)');
legend('Matlab result without speed limits','FW result considering min and max speed','Location','northwest');
title('Target Results');

subplot(2,1,2);
plot(1:frame_count,doppler_level_complete);
hold on;
plot([0, frame_count],[doppler_threshold, doppler_threshold]);
xlabel('Frame');
ylabel('Doppler Level');
legend('Doppler level','Doppler threshold','Location','northwest');

