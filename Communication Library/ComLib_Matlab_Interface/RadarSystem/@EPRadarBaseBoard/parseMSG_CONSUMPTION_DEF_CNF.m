function parseMSG_CONSUMPTION_DEF_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
    % extract parameters from message
	msgsize = protocol_read_payload_uint16(payload, offset);
    offset = offset + 2;

	% read string list
	num_elem = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	
	labels = [];
	
	for index = 1:num_elem
		[ label, len] = protocol_read_payload_str(payload, offset);
		offset = offset + len;
		labels = [labels, label];
    end
    obj.m_consumption_def = labels;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
