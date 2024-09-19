function [RX_startbyte, RX_endpointID, RX_content] = receiveMessage(obj)

% First the message header is read, either using the native serial
% communication or using a Matlab serial object
if isempty(obj.hSerialPort)
    header = serial_io('read', 4);
else
    header = cast(fread(obj.hSerialPort, 4, 'uint8'), 'uint8')';
end

RX_startbyte = 0; 
RX_endpointID = 0;
RX_content = [];

if( length(header) > 0)

    % Header information is extracted
    RX_startbyte = header(1);
    RX_endpointID = header(2);

    if (RX_startbyte == obj.cStartbyteMessage)
        % In a normal message, the header is followed by the payload and the
        % end-of-message marker, so there are read, too.
        RX_payloadSize = cast(typecast(header(3:4), 'uint16'), 'double');

        % Again, either native or Matlab serial communication must be used
        if isempty(obj.hSerialPort)
            RX_content = serial_io('read', RX_payloadSize);
            RX_payloadEODB = typecast(serial_io('read', 2), 'uint16');
        else
            RX_content = cast(fread(obj.hSerialPort, RX_payloadSize, 'uint8'), 'uint8')';
            RX_payloadEODB = fread(obj.hSerialPort, 1, 'uint16');
        end

        % check if message was received correctly
        if (RX_payloadEODB ~= obj.cEndOfMessage)
            disp('[RadarSystem.receiveMessage] Error: Bad message end sequence received')
        end

    elseif (RX_startbyte == obj.cStartbyteStatus)
        % In a status message, the status code is already part of the header so
        % no more data has to be read.
        RX_content = typecast(header(3:4), 'uint16');

    else
        disp('[RadarSystem.receiveMessage] Error: Bad message start byte received')
    end
end
