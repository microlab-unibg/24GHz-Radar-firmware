function consumption = get_consumption(obj)

cmd_message = uint8(zeros(1,5));

cmd_message = protocol_write_payload_uint8(cmd_message, 0, obj.MSG_CONSUMPTION_REQ);
cmd_message = protocol_write_payload_uint32(cmd_message, 1, 1);

obj.m_consumption_pending = false;
obj.doTransmission(cmd_message);
while(~obj.m_consumption_pending) 
	obj.oRS.processResponse; 
end

obj.doTransmission(cmd_message);

consumption = obj.m_consumption;
