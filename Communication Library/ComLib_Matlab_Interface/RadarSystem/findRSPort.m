function [szPort, szDeviceInfo] = findRSPort()

szPort=[];
szDeviceInfo=[];

PortList=getAvailablePorts();

for n=1:length(PortList)
    hSerialPort = serial(PortList{n}); %#ok<TNMLP>
    try %#ok<TRYNC>
        hSerialPort.OutputBufferSize=65536;
        fopen(hSerialPort);
        
        startbyte = hex2dec('5A');
        endpointID = 0;
        payload = hex2dec('0'); % Query Endpoint info
        endOfMessage = hex2dec('E0DB');
        
        message = [ ...
            typecast(cast(startbyte,        'uint8'),'uint8'),  ... % Every message starts with this data
            typecast(cast(endpointID,       'uint8'),'uint8'),  ... % Send endpoint ID as 8 bit integer
            typecast(cast(length(payload),  'uint16'),'uint8'), ... % Send size of payload as 16 bit integer (so maximum payload size is 64kb)
            typecast(cast(payload,          'uint8'),'uint8'),  ... % Send payload
            typecast(cast(endOfMessage,     'uint16'),'uint8'), ... % Every message ends with this data
            ];
        
        % try kind of flush
        if( hSerialPort.BytesAvailable) 
            x = fread(hSerialPort, hSerialPort.BytesAvailable,'uint8');
        end
        fwrite(hSerialPort, message, 'uint8'); % write message to serial port
        pause(0.25)
        % try kind of flush
        if( hSerialPort.BytesAvailable) 
            x = fread(hSerialPort, hSerialPort.BytesAvailable,'uint8');
        end
        fwrite(hSerialPort, message, 'uint8'); % write message to serial port
        pause(0.5)
        
        % receive
        if (hSerialPort.BytesAvailable>=7)
            RX_startbyteMSG = fread(hSerialPort, 1, 'uint8');
            RX_endpointID 	= fread(hSerialPort, 1, 'uint8');
            
            if ([RX_startbyteMSG, RX_endpointID] ==...
                    [startbyte, endpointID]) %#ok<*BDSCA>
                RX_payloadSize  = fread(hSerialPort, 1, 'uint16');
                RX_payload      = fread(hSerialPort, RX_payloadSize, 'uint8');
                RX_endOfMessageMSG = fread(hSerialPort, 1, 'uint16');
                
                if ([RX_payload(1), RX_endOfMessageMSG] ==...
                        [hex2dec('0'), endOfMessage]) %#ok<*BDSCA>            
                    % flush status message from serial buffer
                    fread(hSerialPort, hSerialPort.BytesAvailable, 'uint8');
                    % copy determined port number
                    szPort = PortList{n};
                    fclose(hSerialPort);
                    delete(hSerialPort);
                    return;
                    
                end
            end
        end
    end % try
    fclose(hSerialPort);
    delete(hSerialPort);
end

if isempty(szPort)
    disp('[findRSPort] Error: No Radar System detected.')
end
