function portListDone = resetRS(szPort)

if nargin
    clearSP(szPort); % only clear a single port
else
    clearSP; % clear all ports
end

pause(.1);

if nargin
    portList={szPort}; % prepare communication with only one port
else
    portList = getAvailablePorts();
end

portListDone = {};
for n=1:length(portList) % open every available port and send CMD_STOP
%     hSerialPort = serial(portList{n}); %#ok<TNMLP>
% try %#ok<TRYNC>
%     fopen(hSerialPort);
% 
%     payload = [typecast(cast(2, 'uint8'),'uint8'), ...
%                typecast(cast(0, 'single'),'uint8')];
% 
%     startbyte = hex2dec('5A');
%     endpointID = 1;
%     endOfMessage = hex2dec('E0DB');
% 
%     message = [ ...
%         typecast(cast(startbyte,        'uint8'),'uint8'),  ... % Every message starts with this data
%         typecast(cast(endpointID,       'uint8'),'uint8'),  ... % Send endpoint ID as 8 bit integer
%         typecast(cast(length(payload),  'uint16'),'uint8'), ... % Send size of payload as 16 bit integer (so maximum payload size is 64kb)
%         typecast(cast(payload,          'uint8'),'uint8'),  ... % Send payload
%         typecast(cast(endOfMessage,     'uint16'),'uint8'), ... % Every message ends with this data
%         ];
% 
%     fwrite(hSerialPort, message, 'uint8'); % write message to serial port
%     pause(0.1);
% 
    portListDone = [portListDone; portList(n)]; %#ok<*AGROW>
% end % try
%     fclose(hSerialPort);
%     delete(hSerialPort);
end

if ~isempty(portListDone)
	disp('[resetRS] Reset the following port(s):')
    disp(portListDone);
end

    