%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Copyright (C) 2014-2021 Infineon Technologies AG
% All rights reserved.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This document contains proprietary information of Infineon Technologies AG.
% Passing on and copying of this document, and communication of its contents
% is not permitted without Infineon's prior written authorisation.
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
% SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
% INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
% CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
% ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
% POSSIBILITY OF SUCH DAMAGE.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function apply_parameters(obj, param_new)

temp_param = obj.m_read_parameters;
if nargin == 1
    param_new = obj.m_new_parameters;
    [status, msg] = obj.check_parameter_limits( param_new );
    if( status ~= 0)
        error(msg); 
    end
    
    obj.m_new_parameters = {};
else
    [status, msg] = obj.check_parameter_limits( param_new );
    if( status ~= 0)
        error(msg); 
    end
end




if isfield(param_new, 'max_speed_mps')
   temp_param.max_speed_mps = param_new.max_speed_mps;
end

if isfield(param_new, 'min_speed_mps')
   temp_param.min_speed_mps = param_new.min_speed_mps;
end

if isfield(param_new, 'frame_time_sec')
    temp_param.frame_time_sec = param_new.frame_time_sec;
end
if isfield(param_new, 'number_of_samples')
    temp_param.number_of_samples = param_new.number_of_samples;
end

if isfield(param_new, 'sampling_freq_hz')
    temp_param.sampling_freq_hz = param_new.sampling_freq_hz;
end

if isfield(param_new, 'doppler_sensitivity')
    temp_param.doppler_sensitivity = param_new.doppler_sensitivity;
end

if isfield(param_new, 'motion_sensitivity')
   temp_param.motion_sensitivity = param_new.motion_sensitivity;
end

if isfield(param_new, 'baseband_gain_stage')
    temp_param.baseband_gain_stage = param_new.baseband_gain_stage;
end

if isfield(param_new, 'continuous_mode')
   temp_param.continuous_mode = param_new.continuous_mode;
end

if isfield(param_new, 'number_of_skip_samples')
    temp_param.number_of_skip_samples = param_new.number_of_skip_samples;
end
offset = 0;

cmd_message = uint8(zeros(1,35));
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, obj.MSG_PARA_SET_VALUES_REQ);
offset = offset + 1;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.max_speed_mps);
offset = offset + 4;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.min_speed_mps);
offset = offset + 4;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.frame_time_sec);
offset = offset + 4;
cmd_message = protocol_write_payload_uint16(cmd_message, offset, temp_param.number_of_samples);
offset = offset + 2;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.sampling_freq_hz);
offset = offset + 4;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.doppler_sensitivity);
offset = offset + 4;
cmd_message = protocol_write_payload_float (cmd_message, offset, temp_param.motion_sensitivity);
offset = offset + 4;
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, temp_param.baseband_gain_stage);
offset = offset + 1;
cmd_message = protocol_write_payload_uint8 (cmd_message, offset, temp_param.equistantant_mode);
offset = offset + 1;
cmd_message = protocol_write_payload_uint16(cmd_message, offset, temp_param.number_of_skip_samples);
offset = offset + 2;
cmd_message = protocol_write_payload_uint32(cmd_message, offset, temp_param.pulse_width_usec);
offset = offset + 4;
if( offset ~= length(cmd_message))
   error('length mismatch!'); 
end

obj.m_read_parameters = {};
obj.doTransmission(cmd_message, 2, 0.1);
obj.m_last_frame_time_sec = temp_param.frame_time_sec;
while(isempty(obj.m_read_parameters))
    obj.oRS.processResponse;
end

