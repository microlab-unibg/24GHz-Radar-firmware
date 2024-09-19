function parseMSG_BOARD_INFO_CNF(obj, payload)

if (length(payload) > 1)
	offset = 1;
    % extract parameters from message
    board_info.major_version = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	board_info.minor_version = protocol_read_payload_uint8(payload, offset);
	offset = offset + 1;
	[ board_info.rf_shield_type_id, len] = protocol_read_payload_str(payload, offset);
    offset = offset + len;
    [ board_info.description, len] = protocol_read_payload_str(payload, offset);
    offset = offset + len;

    obj.m_major_version =board_info.major_version;
    obj.m_minor_version = board_info.minor_version;
	obj.m_rf_shield_type_id = board_info.rf_shield_type_id;
	obj.m_description  = board_info.description;
    
    obj.m_board_info = board_info;
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
