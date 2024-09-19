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
function parseMSG_PARA_VALUES_DEF_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
    % extract parameters from message
	msgsize = protocol_read_payload_uint16(payload, offset);
    offset = offset + 2;

	% read string list
	para_def.max_speed_mps_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.max_speed_mps_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.min_speed_mps_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.min_speed_mps_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.frame_time_sec_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.frame_time_sec_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;

	% read samples list
	num_elem = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	
	samples = [];
	
	for index = 1:num_elem
		elem = protocol_read_payload_uint16(payload, offset);
		offset = offset + 2;
		samples = [samples, elem];
    end 

	para_def.sample_list = samples;
	
	para_def.sampling_freq_hz_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.sampling_freq_hz_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.doppler_sensitivity_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.doppler_sensitivity_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.motion_sensitivity_lower_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para_def.motion_sensitivity_upper_boundary = protocol_read_payload_float(payload, offset);
	offset = offset + 4;

	para_def.number_of_skip_samples_lower_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	para_def.number_of_skip_samples_upper_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
    para_def.pulse_width_usec_lower_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	para_def.pulse_width_usec_upper_boundary = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
	
    obj.m_parameters_def = para_def;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
