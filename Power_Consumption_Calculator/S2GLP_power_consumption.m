%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Script S2GLP_power_consumption.m
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
% disclaimer in the documentation and/or other materials provided with the
% distribution.cls
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
% This script calculates the power consumption of the S2GLP RF shield based on the selected radar parameters.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Start-up
clear;
clc;

%% Radar Settings (Default Mode)
frame_time_ms = 150;

transient_skip_count =  40;
number_of_samples    = 128;

total_samples_per_frame = transient_skip_count + number_of_samples;

fS = 2000;

EQ_MODE = 0;

%% Radar Settings (Low Power Mode)
% frame_time_ms = 200;
% 
% transient_skip_count = 25;
% number_of_samples    = 64;
% 
% total_samples_per_frame = transient_skip_count + number_of_samples;
% 
% fS = 2000;
% 
% EQ_MODE = 0;

%% Radar Settings (High Performance Mode)
% frame_time_ms = 64; % automatically set
% 
% transient_skip_count =   0; % automatically set
% number_of_samples    = 256;
% 
% total_samples_per_frame = transient_skip_count + number_of_samples;
% 
% fS = 4000;
% 
% EQ_MODE = 1;

%% Duty Cycle (DC) Settings
TIME_DMA_TRANSFER_MS =   2.0;
TIME_INIT_PTAT_MS    =   1.0;
TIME_INIT_BB_MS      =  20.0;
TIME_BGT_ON_MS       =  5e-3;
TIME_PTAT_ON_MS      = 20e-3;

TIME_BB_ON_MS = TIME_INIT_BB_MS + (number_of_samples - 1) / fS * 1e3 + TIME_DMA_TRANSFER_MS;
if TIME_BB_ON_MS > frame_time_ms
    TIME_BB_ON_MS = frame_time_ms;
end

if EQ_MODE == 1
    total_samples_per_frame = number_of_samples; % transient_skip_count = 0;
    frame_time_ms     = number_of_samples / fS * 1e3;
    TIME_BB_ON_MS     = frame_time_ms;
    TIME_INIT_PTAT_MS = 0;
end

DC_INIT_PTAT_ON = TIME_INIT_PTAT_MS / frame_time_ms;
DC_BGT_ON       = total_samples_per_frame * TIME_BGT_ON_MS  / frame_time_ms;
DC_PTAT_ON      = total_samples_per_frame * TIME_PTAT_ON_MS / frame_time_ms;
DC_BB_ON        = TIME_BB_ON_MS / frame_time_ms;

%% Current Consumption (CC) Constants
CC_BGT_mA   = 45;
CC_DIV_mA   = 19;
CC_PTAT_mA  = 1.5;
CC_BB_mA    = 0.335; % TLV9002 (OPA) + Voltage Divider

VCC_V = 3.3;

%% Calculations
current_consumption_mA_init    = DC_INIT_PTAT_ON * CC_PTAT_mA;
current_consumption_mA_samples = DC_BGT_ON * CC_BGT_mA + DC_PTAT_ON * CC_PTAT_mA + DC_BB_ON * CC_BB_mA;
current_consumption_mA_total   = current_consumption_mA_init + current_consumption_mA_samples;

power_consumption_mW_total = current_consumption_mA_total * VCC_V;

disp(['S2GLP Current Consumption: ', num2str(current_consumption_mA_total), ' mA']);
disp(['        Power Consumption: ', num2str(power_consumption_mW_total), ' mW']);

