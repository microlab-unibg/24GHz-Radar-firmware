function val = protocol_read_payload_uint64 (payload, ind_zb)

val = typecast(payload(ind_zb+1:ind_zb+8), 'uint64');
