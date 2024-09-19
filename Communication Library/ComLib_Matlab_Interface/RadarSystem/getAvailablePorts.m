function cPorts = getAvailablePorts()
% function lCOM_Port = getAvailableComPort()
% Return a Cell Array of COM port names available on your computer

try
    s=serial('DUMMY_PORT_NAME');
    fopen(s); 
catch ME
    lErrMsg = ME.message;
    % examples:
    % lErrMsg = [['Open failed: Port: DUMMY_PORT_NAME is not available. No ports are available.' char(10) 'Use INSTRFIND to determine if other instrument objects are connected to the requested device.']]
    % lErrMsg = [ 'Open failed: Port: DUMMY_PORT_NAME is not available. Available ports: COM3. Use INSTRFIND to determine if other instrument objects are connected to the requested device.']
    % lErrMsg = [['Open failed: Port: DUMMY_PORT_NAME is not available. Available ports: COM3, COM4, COM11.' char(10) 'Use INSTRFIND to determine if other instrument objects are connected to the requested device.']]
end

% close and delete the virtual port as it is no longer needed
fclose(s);
delete(s);

% parse strings
keyword_start='Available ports: ';
keyword_end='Use ';
% start of the COM available port 
lIndex1 = strfind(lErrMsg,keyword_start)+length(keyword_start); 
% end of COM available port
lIndex2 = strfind(lErrMsg,keyword_end)-3; % -3 because of '. U'

% mark the resulting string
lComStr = lErrMsg(lIndex1:lIndex2);
lIndexDot = strfind(lComStr,',');

% segment the string
if isempty(lComStr)
    cPorts={};
else
    lIndexDot = [-1 lIndexDot length(lComStr)+1]; % add virtual markers to make the loop a one shot
    for i=1:length(lIndexDot)-1
        cPorts{i,1} = lComStr(lIndexDot(i)+2:lIndexDot(i+1)-1); %#ok<AGROW>
    end
end
