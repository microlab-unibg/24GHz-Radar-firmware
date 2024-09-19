function board_info = get_board_info(obj)

obj.doTransmission(obj.MSG_BOARD_INFO_REQ);

board_info = obj.m_board_info;


