function consumption_labels = get_consumption_def(obj)

obj.doTransmission(obj.MSG_CONSUMPTION_DEF_REQ);

consumption_labels  = obj.m_consumption_def;

