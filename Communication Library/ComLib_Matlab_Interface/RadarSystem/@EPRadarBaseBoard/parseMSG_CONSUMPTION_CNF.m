function parseMSG_CONSUMPTION_CNF(obj, payload)

if (length(payload) > 1)
		
	offset = 1;
    % extract parameters from message
	msgsize = protocol_read_payload_uint16(payload, offset);
    offset = offset + 2;
	
	num_elem = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	
	consumptions = [];
	
	for index = 1:num_elem
		elem.index = protocol_read_payload_uint8(payload, offset);
		offset = offset + 1;

		elem.value = protocol_read_payload_float(payload, offset);
		offset = offset + 4;
		
		[ elem.unit , len] = protocol_read_payload_str(payload, offset);
		offset = offset + len;
		consumptions = [consumptions, elem];
    end
    obj.m_consumption = consumptions;
	obj.m_consumption_pending = true;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
