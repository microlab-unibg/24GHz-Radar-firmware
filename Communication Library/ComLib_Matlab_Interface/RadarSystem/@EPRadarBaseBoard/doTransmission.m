function doTransmission(obj, message)

% send message
obj.oRS.sendMessage(obj.uEndpointID, message)

% get status message
obj.oRS.processResponse;
