function val = protocol_read_payload_int8 (payload, ind_zb)

val = typecast(payload(ind_zb+1), 'int8');
