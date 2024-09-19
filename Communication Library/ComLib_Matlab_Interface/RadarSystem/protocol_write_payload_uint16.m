function cmd_message = protocol_write_payload_uint16(cmd_message_in, ind_zb, val)

cmd_message = cmd_message_in;

% ind_zb is a zero based index
cmd_message(1, (ind_zb+1):(ind_zb+2)) = typecast(uint16(val),'uint8');
