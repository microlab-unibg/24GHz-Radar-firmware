function cmd_message = protocol_write_payload_float(cmd_message_in, ind_zb, val)

cmd_message = cmd_message_in;

% ind_zb is a zero based index
cmd_message(1, (ind_zb+1):(ind_zb+4)) = typecast(single(val),'uint8');

