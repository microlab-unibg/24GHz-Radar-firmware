function parseMSG_PARA_VALUES_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
    % extract parameters from message

	% read string list
	para.max_speed_mps = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para.min_speed_mps = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para.frame_time_sec = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	
	para.number_of_samples = protocol_read_payload_uint16(payload, offset);
	offset = offset + 2;
		
	para.sampling_freq_hz = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para.doppler_sensitivity = protocol_read_payload_float(payload, offset);
	offset = offset + 4;
	para.motion_sensitivity = protocol_read_payload_float(payload, offset);
	offset = offset + 4;

	para.baseband_gain_stage = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	para.equistantant_mode = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	para.number_of_skip_samples = protocol_read_payload_uint16(payload, offset);
	offset = offset + 2;
	para.pulse_width_usec = protocol_read_payload_uint32(payload, offset);
	offset = offset + 4;
    
    obj.m_read_parameters = para;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end