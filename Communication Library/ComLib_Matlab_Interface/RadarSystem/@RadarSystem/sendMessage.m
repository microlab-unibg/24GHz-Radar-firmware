function sendMessage(obj, uEndpoint, payload)

try
    message = [ ...
        typecast(cast(obj.cStartbyteMessage,    'uint8'),'uint8'),  ... % Every message starts with this data
        typecast(cast(uEndpoint,                'uint8'),'uint8'),  ... % Send endpoint ID as 8 bit integer
        typecast(cast(length(payload),          'uint16'),'uint8'), ... % Send size of payload as 16 bit integer (so maximum payload size is 64kb)
        typecast(cast(payload,                  'uint8'),'uint8'),  ... % Send payload
        typecast(cast(obj.cEndOfMessage,        'uint16'),'uint8'), ... % Every message ends with this data
        ];

    % If there is no serial port object, the native serial communication is
    % used. If that object exist, it is used as a fallback.
    if isempty(obj.hSerialPort)
        serial_io('write', message); % write message to serial port
    else
        fwrite(obj.hSerialPort, message, 'uint8');
    end
    
catch exeption
    disp(['[RadarSystem.sendMessage] Error: Unable to write to USB! Exeption: ', exeption.message])
end