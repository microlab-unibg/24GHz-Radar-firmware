function endpointID = findEndpointID(obj, EndpointType) % DEPRECATED, use getEndpointList instead!

if ischar(EndpointType)
    uEndpointType = typecast(flip(uint8(EndpointType)),'int32');
else
    uEndpointType = EndpointType;
end

try
    % send message to query endpoint list from the device
    % ---------------------------------------------------
    endpoint = 0;
    message = 0;
    obj.sendMessage(endpoint, message);
    
    % read endpoint table from device
    % -------------------------------
    
    % receive startbyte, endpoint ID and payload
    [RX_startbyte, RX_endpointID, RX_payload] = obj.receiveMessage;
    
    % parse payload, check command byte and size
    if ((RX_startbyte == obj.cStartbyteMessage) && (RX_endpointID == 0))
        
        numEndpoints = RX_payload(2);
        
        if ((RX_payload(1) ~= 0) && (RX_payloadSize == 2 + 6 * numEndpoints))
            disp('[RadarSystem.findEndpointID] Error: Could not get endpoint table!')
            return;
        end
        
        EndpointTypes = uint32(zeros(1,numEndpoints));
        EndpointTags = cell(1,numEndpoints);
        EndpointVersions = uint16(zeros(1, numEndpoints));
        
        for i = 1:numEndpoints
            baseindex = (i-1)*6 + 2;
            EndpointTypes(i) = uint32(typecast(RX_payload( baseindex + 1: baseindex + 4), 'uint32'));
            EndpointVersions(i) = uint16(typecast(RX_payload( baseindex + 5: baseindex + 6), 'uint16'));
            EndpointTags{i} = char(flip(typecast(EndpointTypes(i),'uint8')));
        end
        
        endpointID = find(EndpointTypes == uEndpointType);
    end
    
    % flush serial buffer, because there should be a status message
    % available now
    [~, ~, ~] = obj.receiveMessage; % drop return values, could be evaluated in later versions, if needed
    
catch exeption
    disp(['[RadarSystem.findEndpointID] Error: Unable to write to USB! Exception: ', exeption.message])
end
