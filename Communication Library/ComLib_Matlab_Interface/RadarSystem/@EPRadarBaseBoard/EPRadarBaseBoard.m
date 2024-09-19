 classdef EPRadarBaseBoard < handle

    %% methods
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods (Hidden) % constructor, general class
        function obj = EPRadarBaseBoard(oRS, uEndpointID, uEndpointVer)
            obj.uEndpointID = uEndpointID;
            obj.uEndpointVer = uEndpointVer;
            obj.oRS = oRS;
        end

        function initValues(obj)
            obj.get_board_info;
			obj.get_consumption_def;
			obj.get_consumption;
        end

        parsePayload(obj, RX_content);
        doTransmission(obj, message);
    end

    methods (Hidden) % communication with endpoint
		major_version = get_major_version(obj)
		m_minor_version = get_minor_version(obj)
        rf_shield_type_id = get_rf_shield_type_id(obj)
		description = get_description(obj)
		
		consumption_def = get_consumption_def(obj)
		consumption = get_consumption(obj)
    end

    methods (Hidden) % parsing of incoming messages
        parseMSG_BOARD_INFO_CNF(obj, payload)
		parseMSG_CONSUMPTION_DEF_CNF(obj, payload)
		parseMSG_CONSUMPTION_CNF(obj, payload)
		parseMSG_PARA_FACTORY_RESET_CNF(obj, payload)
    end

	%% properties
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    properties (Hidden, Constant) % endpoint tag
        szEPTag = 'TJBA'; % ep_radar_tjba_definition    0x544A4241
        uMinVer = 1;      % min version of this endpoint on FW supported by this host code
        uMaxVer = 1;      % max version of this endpoint on FW supported by this host code
    end

    properties (Hidden) % general class
        uEndpointID;
        uEndpointVer;
        oRS;
    end

    properties (Hidden) % data properties
		m_major_version;
		m_minor_version;
		m_rf_shield_type_id;
	    m_description;
		
        m_board_info;
		m_consumption_def;
		m_consumption;
		m_consumption_pending;
    end
    
    properties (Dependent)
		major_version;
		minor_version;
		rf_shield_type_id;
	    description;
		
		consumption_def;
		consumption;
	end

    properties (Access = private, Constant)	% message ids
        MSG_BOARD_INFO_REQ 		 = uint8(hex2dec('E0')); 
        MSG_BOARD_INFO_CNF       = uint8(hex2dec('E1')); 
		MSG_CONSUMPTION_DEF_REQ  = uint8(hex2dec('D0')); 
		MSG_CONSUMPTION_DEF_CNF  = uint8(hex2dec('D1')); 
		MSG_CONSUMPTION_REQ		 = uint8(hex2dec('D2')); 
		MSG_CONSUMPTION_CNF		 = uint8(hex2dec('D3')); 
		MSG_PARA_FACTORY_RESET_REQ = uint8(hex2dec('E4')); 
		MSG_PARA_FACTORY_RESET_CNF = uint8(hex2dec('E5')); 
    end
    
    %% set/get methods
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods % set/get
        function val = get.major_version(obj)
            val = obj.m_major_version;
        end
        function val = get.minor_version(obj)
            val = obj.m_minor_version;
        end
        function val = get.rf_shield_type_id(obj)
            val = obj.m_rf_shield_type_id;
        end
        function val = get.description(obj)
            val = obj.m_description;
        end
		function val = get.consumption_def(obj)
			val = obj.m_consumption_def;
		end
		function val = get.consumption(obj)
			val = obj.m_consumption;
        end
    end
    
end
