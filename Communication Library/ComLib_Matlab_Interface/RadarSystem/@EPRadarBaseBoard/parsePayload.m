function parsePayload(obj, RX_content)

switch RX_content(1) % command code
    case obj.MSG_BOARD_INFO_CNF
        obj.parseMSG_BOARD_INFO_CNF(RX_content)
	case obj.MSG_CONSUMPTION_DEF_CNF
		obj.parseMSG_CONSUMPTION_DEF_CNF(RX_content)
	case obj.MSG_CONSUMPTION_CNF
		obj.parseMSG_CONSUMPTION_CNF(RX_content)
    case obj.MSG_PARA_FACTORY_RESET_CNF
        obj.parseMSG_PARA_FACTORY_RESET_CNF(RX_content)
	otherwise
        fprintf('[%s.%s] Error! Unknown Command Code: 0x%s\n',mfilename('Class'),mfilename,dec2hex(RX_content(1)));
end
