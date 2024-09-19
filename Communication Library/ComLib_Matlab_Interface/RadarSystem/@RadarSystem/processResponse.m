function processResponse(obj)

RX_startbyte = -1; % init to run while loop at least once
while(RX_startbyte~=obj.cStartbyteStatus && RX_startbyte~=obj.cStartbyteMessage)
    % fetch new message
    [RX_startbyte, RX_endpointID, RX_content] = obj.receiveMessage;
    if length(RX_content) > 0

        % find endpoint and call action
        ind = find(obj.uEndpointIDs == RX_endpointID);
        if (~isempty(ind))
            if(RX_startbyte==obj.cStartbyteMessage)
                obj.cEndpoints{ind}.parsePayload(RX_content);
            elseif(RX_startbyte==obj.cStartbyteStatus)
               if (RX_content ~= 0)
                    obj.parseEPStatus(RX_content); % future option: call individual parser method for each endpoint: obj.cEndpoints{ind}.parseStatus(RX_content);
                end
            else
                fprintf('[%s.%s] Error! Unknown Startbyte: 0x%s\n',mfilename('Class'),mfilename,dec2hex(RX_startbyte));
            end
        else
            fprintf('[%s.%s] Error! Unknown Endpoint: 0x%s\n',mfilename('Class'),mfilename,dec2hex(RX_endpointID));
        end
    end
end
