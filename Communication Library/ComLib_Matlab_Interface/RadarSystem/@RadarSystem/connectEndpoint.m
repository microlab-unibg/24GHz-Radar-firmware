function connectEndpoint(obj, szEndpointClass, szEndpointTag)

szEndpointObject = ['o' szEndpointClass];
uEnpointID = obj.findEndpointID(szEndpointTag);
eval(sprintf('obj.%s = %s(obj, uEnpointID)', szEndpointObject, szEndpointClass));
obj.cEndpoints{end+1} = obj.(szEndpointObject);
obj.uEndpointIDs(end+1) = uEnpointID;
