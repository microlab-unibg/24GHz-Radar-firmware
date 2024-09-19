function clearSP(szPort)

if nargin
    oSP=instrfind('Port',szPort); % just prepare single port for clearing
else
    oSP=instrfind; % prepare all ports for clearing
end

if ~isempty(oSP)
    fclose(oSP);
    delete(oSP);
end
