function parseMSG_CONSUMPTION_DEF_CNF(obj, payload)

if (length(payload) > 0)
	offset = 1;
   % message does not contain any value
else
    fprintf('[%s.%s] Error! Wrong payload size.\n',mfilename('Class'),mfilename);
end
