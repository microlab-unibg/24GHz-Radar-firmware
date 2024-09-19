classdef RadarSystem < handle & dynamicprops

    %% methods: constructor, destructor
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods % constructor & destructor
        % The constructor gets a COM port and synchronizes the data from
        % the board with the properties
        function obj = RadarSystem(szPort)

            % User may have specified server and port for a UDP connection
            % rather than a COM port. To check this it's tried to extract
            % that information from the port name
            remote_host = regexp(szPort, ...
                                 'udp://([A-Za-z0-9.]*):([0-9]*)', ...
                                 'tokens', 'once');
            
            if (length(remote_host) == 2)
                % Parsing of the port name was succesful, so user specified
                % a UDP socket address. A UDP port is now opened and used
                % just as if it were a serial object.
                udp_host = remote_host{1};
                udp_port = str2double(remote_host{2});
                obj.hSerialPort = udp(udp_host, udp_port);
                obj.hSerialPort.DatagramTerminateMode = 'off';
                obj.hSerialPort.ByteOrder = 'littleEndian';
                obj.hSerialPort.InputBufferSize = 65536*8;
                fopen(obj.hSerialPort);
            else
                % Parsing of the port name was not succesful, so the user
                % did not specify a UDP socket address. So it is assumed
                % that the specified name is the name of a serial port.
                if (exist('serial_io', 'file') == 3)
                    % If native serial io mex interface is available, it is
                    % used to connect to the radar hardware
                    try
                        serial_io('open', szPort);
                    catch err
                        disp(['[RadarSystem.RadarSystem] Serial connection error: ', err.message]);
                        return
                    end
                else
                    % As a fallback Matlab's serial object is used to connect
                    % to the radar hardware
                    obj.hSerialPort = serial(szPort);

                    try
                        obj.hSerialPort.InputBufferSize = 65536*8;
                        obj.hSerialPort.OutputBufferSize = 65536*8;
                        obj.hSerialPort.Timeout = 5; % s
                        fopen(obj.hSerialPort);
                    catch err
                        disp(['[RadarSystem.RadarSystem] Serial connection error: ', err.message]);
                        fclose(obj.hSerialPort);
                        delete(obj.hSerialPort);
                        return
                    end
                end
            end

            % find potential EP dirs
            szClassPath = mfilename('fullpath');
            ind = strfind(szClassPath, filesep);
            szLibDir = szClassPath(1:ind(end-1));
            sPotentialEPDirs = dir([szLibDir '@*']);

            % query endpoints from board
            [~, cDeviceEPTags, cDeviceEPVersions] = obj.getEndpointList;
            
            % dynamically bind available endpoints
            for n=1:length(sPotentialEPDirs)
                szEPClass = sPotentialEPDirs(n).name(2:end); % dir name w/o the @ sign
                szObj = ['o' szEPClass];
                try
                    szEPTag = eval([szEPClass '.szEPTag']); % check if the class can return an EP tag
                    uMinVer = eval([szEPClass '.uMinVer']);
                    uMaxVer = eval([szEPClass '.uMaxVer']);
                catch
                    continue
                end

                uEPID = find(strcmp(cDeviceEPTags, szEPTag)); % check if the device provides the specific EP
                uEPVer = cDeviceEPVersions(uEPID);
                if ~isempty(uEPID) % if the device supports the specific endpoint
                    if ((uMinVer <= uEPVer)&&(uMaxVer >= uEPVer))
                        obj.addprop(szObj);
                        obj.(szObj) = eval(sprintf('%s(obj,%d,%d);', szEPClass, uEPID, uEPVer));
                    
                        obj.cEndpoints{end+1} = obj.(szObj);
                        obj.uEndpointIDs(end+1) = uEPID;
                    else
                        error('Endpoint not supported on host OR Endpoint version on FW incompatible with host');
                    end
                end
            end
            
            % init endpoints
            for n=1:length(obj.cEndpoints)
                obj.cEndpoints{n}.initValues;
            end

        end

        % The destructor closes the COM port and deletes the internal serial object.
        function delete(obj)
            if ~isempty(obj.hSerialPort) && isvalid(obj.hSerialPort)
                fclose(obj.hSerialPort);
                delete(obj.hSerialPort);
            elseif (exist('serial_io', 'file') == 3)
                serial_io('close');
            end
        end
        
        function bytes = BytesAvailable(obj)
            if isempty(obj.hSerialPort)
                bytes = -1; 
            else
                bytes = obj.hSerialPort.BytesAvailable; 
            end
        end
	end
    
	methods (Static)%, Access = private)
        parseEPStatus(uErrorCode);
    end

    %% properties
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    % general class
    properties (Access = private, Constant)
        % constants that are part of protocol messages
        cStartbyteMessage   = uint8(hex2dec('5A'));
        cStartbyteStatus    = uint8(hex2dec('5B'));
        cStartbyteError     = uint8(hex2dec('5B'));
        cEndOfMessage       = uint16(hex2dec('E0DB'));
    end
    
    properties (Hidden)
        hSerialPort; % a handle to the serial port object used for communication
        uEndpointIDs
        cEndpoints
    end
    
end
