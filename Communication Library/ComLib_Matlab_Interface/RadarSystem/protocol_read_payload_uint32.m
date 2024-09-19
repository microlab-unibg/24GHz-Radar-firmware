function val = protocol_read_payload_uint32 (payload, ind_zb)

val = typecast(payload(ind_zb+1:ind_zb+4), 'uint32');
