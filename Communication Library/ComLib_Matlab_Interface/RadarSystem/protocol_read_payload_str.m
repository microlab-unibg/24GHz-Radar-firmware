function [val, len] = protocol_read_payload_str (payload, ind_zb)

len = typecast(payload(ind_zb+1)+1, 'uint8'); 

val = char(payload(ind_zb+2:ind_zb+len));

