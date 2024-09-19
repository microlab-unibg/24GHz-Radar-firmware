%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function out = PlotDemo (in)
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
% This script showcases the acquisition and live visualization of raw data and records the target velocity.
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
oRS.oEPRadarS2GLP.parameters.number_of_samples = NTS;
oRS.oEPRadarS2GLP.parameters.frame_time_sec = 0.1500;
oRS.oEPRadarS2GLP.parameters.min_speed_mps = 0.3;

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
i = 1;

[mxResultData(i), mxRawData] = oRS.oEPRadarS2GLP.get_result_and_raw_data;
fprintf('Frame number: %d', mxRawData.frame_number);

subplot(2,1,1);
plot_data = [real(mxRawData.sample_data(:,1)), imag(mxRawData.sample_data(:,1))];
hData = plot(plot_data);
xlim([1,NTS]);
ylim([0,1]);
xlabel('Sample');
ylabel('ADC Value (FSR)');
title('Raw Data');
legend(['I';'Q'],'Location','northeastoutside');

subplot(2,1,2);
if mxResultData(i).is_target_approaching == 1
    fill([0,1,1,0],[0,0,1,1],'green');
    text(0.5,0.5,'Target approaching','HorizontalAlignment','center','FontSize',18);
    fprintf(', Target approaching: %.2f m/s\n',mxResultData(i).velocity_mps);
elseif mxResultData(i).is_target_departing == 1
    fill([0,1,1,0],[0,0,1,1],'red');
    text(0.5,0.5,'Target departing','HorizontalAlignment','center','FontSize',18,'Color','white');
    fprintf(', Target departing: %.2f m/s\n',mxResultData(i).velocity_mps);
elseif mxResultData(i).is_motion_detected == 1
    fill([0,1,1,0],[0,0,1,1],'blue');
    text(0.5,0.5,'Motion detected','HorizontalAlignment','center','FontSize',18,'Color','white');
    fprintf(', Motion detected\n');
else
    fill([0,1,1,0],[0,0,1,1],'white');
    text(0.5,0.5,'No motion','HorizontalAlignment','center','FontSize',18);
    fprintf('\n');
end
axis off;

drawnow;

%%% Start infinite loop to get and plot raw data and emulate LED
while ishandle(hFig)
    
    [mxResultData(i), mxRawData] = oRS.oEPRadarS2GLP.get_result_and_raw_data;
    fprintf('Frame number: %d',mxRawData.frame_number);
    
    subplot(2,1,1);
    plot_data = [real(mxRawData.sample_data(:,1)), imag(mxRawData.sample_data(:,1))];
    plot_data_cell = mat2cell(transpose(plot_data),ones(1,2));
    set(hData,{'YData'},plot_data_cell);
    
    subplot(2,1,2);
    if mxResultData(i).is_target_approaching == 1
        fill([0,1,1,0],[0,0,1,1],'green');
        text(0.5,0.5,'Target approaching','HorizontalAlignment','center','FontSize',18);
        fprintf(', Target approaching: %.2f m/s\n',mxResultData(i).velocity_mps);
    elseif mxResultData(i).is_target_departing == 1
        fill([0,1,1,0],[0,0,1,1],'red');
        text(0.5,0.5,'Target departing','HorizontalAlignment','center','FontSize',18,'Color','white');
        fprintf(', Target departing: %.2f m/s\n',mxResultData(i).velocity_mps);
    elseif mxResultData(i).is_motion_detected == 1
        fill([0,1,1,0],[0,0,1,1],'blue');
        text(0.5,0.5,'Motion detected','HorizontalAlignment','center','FontSize',18,'Color','white');
        fprintf(', Motion detected\n');
    else
        fill([0,1,1,0],[0,0,1,1],'white');
        text(0.5,0.5,'No motion','HorizontalAlignment','center','FontSize',18);
        fprintf('\n');
    end
    axis off;
    
    i = i + 1;
    drawnow;
    
end

%% 5. Plot target velocity of collected data
figure;
plot([mxResultData.velocity_mps]);
xlim([1,i-1]);
xlabel('Frame');
ylabel('Velocity (m/s)');
title('Target Velocity');

%% 6. Clear radar system object
disp('Clear radar object...');
clearSP(szPort);

%% 7. End of script
disp('Done!');

