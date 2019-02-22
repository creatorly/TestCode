#ifdef __cplusplus
extern "C" {
#endif

//#include <utl_string.h>

#include "invoke_acl.h"
#include "invoke_acl_.h"

PST_INVOKE_ACL INVOKE_ACL_Create(PST_CPE_CFG pCfg)
{
    PST_INVOKE_ACL invokeAcl;

    invokeAcl = INVOKE_TALLOC(ST_INVOKE_ACL);
    if (NULL == invokeAcl)
    {
        CPE_LOG_ERROR("invokeAcl create error, size = %d", sizeof(PST_INVOKE_ACL));
        return NULL;
    }
    memset(invokeAcl, 0, sizeof(ST_INVOKE_ACL));
    
    utl_list_init(&invokeAcl->blackList);
    utl_list_init(&invokeAcl->whiteList);
    utl_list_init(&invokeAcl->portForwardList);
    invokeAcl->rule = "ACL_RULES";
    invokeAcl->cfg = pCfg;

    invokeAcl->accessSwitch = CPE_CfgGetInt(pCfg, CPE_CFG_ACCESS_SWITCH);
    invokeAcl->accessMode = CPE_CfgGetInt(pCfg, CPE_CFG_ACCESS_MODE);
    if (invokeAcl->accessSwitch == 1)   
    {
        invokeAclIptablesMacCmd(invokeAcl, "F", NULL, NULL);
	if (invokeAcl->accessMode == 1) {
		invokeAclIptablesMacCmd(invokeAcl, "A", NULL, "DROP");
	} else {
		invokeAclIptablesMacCmd(invokeAcl, "A", NULL, "ACCEPT");
	}  
                
    }
    CPE_LOG_DEBUG("get init value ,invokeAcl->accessSwitch = %d, invokeAcl->accessMode = %d", invokeAcl->accessSwitch, invokeAcl->accessMode);

    int i;
    for (i = 0; i < utl_cfg_size(pCfg, CPE_CFG_BLACK_LIST); i++) {
        char macHostName[512];
        char blackMac[18];
        char hostName[128];
        utl_cfg_get_str_r(pCfg, CPE_CFG_BLACK_LIST, i, macHostName, sizeof(macHostName));

        if (sscanf(macHostName, "%18s %128s", blackMac, hostName) < 2)
        {
            CPE_LOG_WRAN("invalid config!, whiteMac = %s, hostName = %s", blackMac, hostName);
            continue;
        }

        ST_MAC_HOSTNAME *macHostNameSt = invokeAclMacHostNameCreate(blackMac, hostName, i);
        if (invokeAcl->accessSwitch == 1 && invokeAcl->accessMode == 0 && blackMac[0] != '\0') 
        {
            invokeAclIptablesMacCmd(invokeAcl, "I", blackMac, "DROP");
        }

        utl_list_add(&invokeAcl->blackList, &macHostNameSt->node);

    }

    for (i = 0; i < utl_cfg_size(pCfg, CPE_CFG_WHITE_LIST); i++) {
        char macHostName[512];
        char whiteMac[18];
        char hostName[128];

        utl_cfg_get_str_r(pCfg, CPE_CFG_WHITE_LIST, i, macHostName, sizeof(macHostName));

        if (sscanf(macHostName, "%18s %128s", whiteMac, hostName) < 2)
        {
            CPE_LOG_WRAN("invalid config!, whiteMac = %s, hostName = %s", whiteMac, hostName);
            continue;
        }

        ST_MAC_HOSTNAME *macHostNameSt = invokeAclMacHostNameCreate(whiteMac, hostName, i);
        if (invokeAcl->accessSwitch == 1 && invokeAcl->accessMode == 1 && whiteMac[0] != '\0') 
        {
             invokeAclIptablesMacCmd(invokeAcl, "I", whiteMac, "ACCEPT");
        }
        utl_list_add(&invokeAcl->whiteList, &macHostNameSt->node);

    }

    for (i = 0; i < utl_cfg_size(pCfg, CPE_CFG_PORT_FORWARD_LIST); i++) {
	char portForwardStr[1024];
	char ruleIdentification[32];
	char ruleName[128];
	unsigned int sourcePort;
	char protocols[10];
	char destinationIP[16];
	unsigned int destinationPort;
	utl_cfg_get_str_r(pCfg, CPE_CFG_PORT_FORWARD_LIST, i, portForwardStr, sizeof(portForwardStr));
	if (sscanf(portForwardStr, "%32s %128s %d %10s %16s %d", ruleIdentification, ruleName, &sourcePort, protocols, destinationIP, &destinationPort) < 2) {
		CPE_LOG_WRAN("invalid config!, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
				 ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
            	continue;
	}
	
	ST_PORT_FORWARD *portForward = invokeAclPortForwardCreate(ruleIdentification, ruleName, sourcePort, protocols, destinationIP, destinationPort, i);
	invokeAclIptablesPortCmd("I", portForward->sourcePort, portForward->protocols, portForward->destinationIP, portForward->destinationPort);
	utl_list_add(&invokeAcl->portForwardList, &portForward->node);
	
    }

	
    CPE_LOG_DEBUG("invoke acl creat succeed, rule = %s", invokeAcl->rule);
    return invokeAcl;

}

int INVOKE_ACL_Ctrl_Iptables(PST_INVOKE_ACL invokeAcl, char *type, char *ruleName, int setValue, char *hostName, char *mac, char *resultStr, int reult_len)
{
	if (type == NULL || '\0' == *type) {
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", -100, "type is null");
	} else if (ruleName == NULL || '\0' == *ruleName) { 
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", -101, "ruleName is null");
	} else if (strcasecmp(type, "Add") == 0) {
		int result_code = -1;
		int accessMode = 0;
		const char* descStr = NULL;

		if (strcasecmp(ruleName, "White") == 0) {
			accessMode = 1;
		}

		if (NULL == hostName || '\0' == *hostName) {
			hostName = "";
    		}

		if (resultStr != NULL && '\0' != *mac && strlen(mac) == 17) {
			result_code = invokeAclAddMacToList(invokeAcl, mac, hostName, accessMode);
		}

		if (result_code == 0) {
			descStr = "Add Mac succeed";
		} else if (result_code == -1) {
			descStr = "Mac address is invalid";
		} else if (result_code == -11) {
			descStr = "MAC address is already in the list";
		} else {
			descStr = "Unknow error";
		}
		
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", result_code, descStr);
	} else if (strcasecmp(type, "Dec") == 0) {
		int result_code = -1;
		int accessMode = 0;
		const char* descStr = NULL;

		if (strcasecmp(ruleName, "White") == 0) {
			accessMode = 1;
		}

		if (resultStr != NULL && '\0' != *mac && strlen(mac) == 17) {
			result_code = invokeAclRemoveMacFromList(invokeAcl, mac, accessMode);
		}

		if (result_code == 0) {
			descStr = "Remove Mac succeed";
		} else if (result_code == -1) {
			descStr = "Mac address is invalid";
		} else if (result_code == -11) {
			descStr = "MAC address don't find in the list";
		} else {
			descStr = "Unknow error";
		}		

		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", result_code, descStr);
	} else if (strcasecmp(type, "Set") == 0) {
		int result_code = -1;
		const char* descStr = NULL;
		CPE_LOG_DEBUG("start set switch value, ruleName = %s", ruleName);
		if (setValue) {
			setValue = 1;
		}

		if (strcasecmp(ruleName, "Mode") == 0) {
 			if (invokeAcl->accessMode != setValue) {
				invokeAcl->accessMode = setValue;
				CPE_CfgSetInt(invokeAcl->cfg, CPE_CFG_ACCESS_MODE, setValue);
            			CPE_CfgSave(invokeAcl->cfg);
				if (invokeAcl->accessSwitch == 1) {
					invokeAclchangeIptableMode(invokeAcl, invokeAcl->accessMode);
				}
				result_code = 0;
				descStr = "Set access mode succeed";
			} else {
				result_code = 1;
				descStr = "Access mode is same as old mode";
			}
		} else if (strcasecmp(ruleName, "Switch") == 0) {
			CPE_LOG_DEBUG("start set switch value");

			if (invokeAcl->accessSwitch != setValue) {
				invokeAcl->accessSwitch = setValue;
				CPE_CfgSetInt(invokeAcl->cfg, CPE_CFG_ACCESS_SWITCH, setValue);
            			CPE_CfgSave(invokeAcl->cfg);

				invokeAclIptablesMacCmd(invokeAcl, "F", NULL, NULL);
				if (invokeAcl->accessSwitch == 1) {
					invokeAclchangeIptableMode(invokeAcl, invokeAcl->accessMode);
				}
				result_code = 0;
				descStr = "Set access Switch succeed";
			} else {
				result_code = 1;
				descStr = "Access switch is same as old value";
			}

		} else {
			descStr = "rule name is error";
		}
		
		
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\", \"data\":{\"Switch\":%d, \"Mode\":%d}}",
			 result_code, descStr, invokeAcl->accessSwitch, invokeAcl->accessMode);
	} else if (strcasecmp(type, "Get") == 0) {
		char dataJson[1024];
		int result_code = 0;
		char *descStr = NULL;
		memset(dataJson, '\0', sizeof(dataJson));
		if (strcasecmp(ruleName, "White") == 0 || strcasecmp(ruleName, "Black") == 0) {
			int accessMode = 0;
			if (strcasecmp(ruleName, "White") == 0) {
				accessMode = 1;
			}
			
			char listJson[1000];
			int listLen = sizeof(listJson);
			memset(listJson, '\0', listLen);
			invokeAclGetMacAddrList(invokeAcl, accessMode, listJson, listLen);
		
			snprintf(dataJson, sizeof(dataJson), "\"data\":%s", listJson);
		} else if (strcasecmp(ruleName, "Mode") == 0) {
			snprintf(dataJson, sizeof(dataJson), "\"data\":%d", invokeAcl->accessMode);
		} else if (strcasecmp(ruleName, "Switch") == 0){
			snprintf(dataJson, sizeof(dataJson), "\"data\":%d", invokeAcl->accessSwitch);
		} else if (strcasecmp(ruleName, "All") == 0){
			char whiteListJson[512];
			char blackListJson[512];
			int whiteListLen = sizeof(whiteListJson);
			int blackListLen = sizeof(blackListJson);
			memset(whiteListJson, '\0', whiteListLen);
			invokeAclGetMacAddrList(invokeAcl, 1, whiteListJson, whiteListLen);
			memset(blackListJson, '\0', blackListLen);
			invokeAclGetMacAddrList(invokeAcl, 0, blackListJson, blackListLen);
			
			snprintf(dataJson, sizeof(dataJson), "\"data\":{\"whiteList\":%s, \"blackList\":%s, \"Switch\":%d, \"Mode\":%d}",
				 whiteListJson, blackListJson,invokeAcl->accessSwitch, invokeAcl->accessMode);
		} else {
			result_code = -11;
		}

		if (result_code == 0) {
			descStr = "Get succeed";
		} else {
			descStr = "Get failed, rule name is error";
		} 		

		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\", %s}", result_code, descStr, dataJson);
	} else {
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", -102, "error type");
	}	

	return 0;
}

int INVOKE_ACL_Ctrl_Port(PST_INVOKE_ACL invokeAcl, char *type, char *ruleIdentification, char *ruleName, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort, char *resultStr, int reult_len)
{
	if (type == NULL || '\0' == *type) {
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", -100, "type is null");
	} else if (strcasecmp(type, "Add") == 0) {
		int result_code = -1;
		const char* descStr = NULL;
		if (sourcePort > 65535 || destinationPort > 65535 || sourcePort * destinationPort == 0) {
			result_code = -102;
			descStr = "Port is error, Ports can only be in the range of 1-65535";
		} else if (NULL == protocols || '\0' == *protocols) {
			result_code = -104;
			descStr = "Protocols is null";
		} else if (strcasecmp(protocols, "tcp") != 0 && strcasecmp(protocols, "udp") != 0) {
			result_code = -106;
			descStr = "Protocols is error";
		} else if (NULL == destinationIP || '\0' == *destinationIP) {
			result_code = -106;
			descStr = "Destination IP is null";
		} else if (NULL == ruleIdentification || '\0' == *ruleIdentification) {
			result_code = -107;
			descStr = "Rule identification is null";
		}

		if (result_code == -1) {
			result_code = invokeAclAddDnatPortToList(invokeAcl, ruleIdentification, ruleName, sourcePort, protocols, destinationIP, destinationPort);
		}

		if (result_code == 0) {
			descStr = "Add dnat port succeed";
		} else if (result_code == -11) {
			descStr = "dnat port is already in the list";
		} else if (result_code == -12) {
			descStr = "create dnat port struct error";
		}
	
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", result_code, descStr);
	} else if (strcasecmp(type, "Dec") == 0) {
		int result_code = -1;
		const char* descStr = NULL;
		if (NULL == ruleIdentification || '\0' == *ruleIdentification) {
			result_code = -107;
			descStr = "Rule identification is null";
		}

		if (result_code == -1) {
			result_code = invokeAclRemoveDnatPortFromList(invokeAcl, ruleIdentification);
		} 

		if (result_code == 0) {
			descStr = "Add dnat port succeed";
		} else if (result_code == -11) {
			descStr = "dnat port can't find in the list";
		} else if (result_code == -12) {
			descStr = "delete dnat port from cfg error";
		}
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", result_code, descStr);
	} else if (strcasecmp(type, "Get") == 0) {
		char listJson[1000];
		int listLen = sizeof(listJson);
		memset(listJson, '\0', listLen);
		invokeAclGetDnatPortList(invokeAcl, listJson, listLen);		

		snprintf(resultStr, reult_len, "{\"code\":0, \"desc\":\"Get succeed\", \"data\":%s}", listJson);
	} else {
		snprintf(resultStr, reult_len, "{\"code\":%d, \"desc\":\"%s\"}", -102, "error type");
	}

	return 0;
}

void invokeAclchangeIptableMode(PST_INVOKE_ACL invokeAcl, int accessMode)
{
    if (accessMode == 0)
    {
	invokeAclIptablesMacCmd(invokeAcl, "F", NULL, NULL);
        invokeAclIptablesMacCmd(invokeAcl, "A", NULL, "ACCEPT");

        utl_node_t *pn = utl_list_begin(&invokeAcl->blackList);
        while (pn != utl_list_end(&invokeAcl->blackList)) 
        {
            ST_MAC_HOSTNAME *macHostName = (ST_MAC_HOSTNAME *)pn;
            invokeAclIptablesMacCmd(invokeAcl, "I", macHostName->hwaddr, "DROP");
            pn = utl_list_next(pn);
        }
    }
    else
    {
        invokeAclIptablesMacCmd(invokeAcl, "F", NULL, NULL);
        invokeAclIptablesMacCmd(invokeAcl, "A", NULL, "DROP");

        utl_node_t *pn = utl_list_begin(&invokeAcl->whiteList);
        while (pn != utl_list_end(&invokeAcl->whiteList))
        {
            ST_MAC_HOSTNAME *macHostName = (ST_MAC_HOSTNAME *)pn;
            invokeAclIptablesMacCmd(invokeAcl, "I", macHostName->hwaddr, "ACCPET");
            pn = utl_list_next(pn);
        }
    }
}

ST_MAC_HOSTNAME * invokeAclMacHostNameCreate(const char* macAddr, const char *hostName, int cfgIndex)
{
    ST_MAC_HOSTNAME *macHostName;
    macHostName = INVOKE_TALLOC(ST_MAC_HOSTNAME);
    if (macHostName == NULL) {
        CPE_LOG_ERROR("init ST_MAC_HOSTNAME sturct error,macAddr = %s, hostName : %s", macAddr, macAddr);
        return NULL;
    }
    memset(macHostName, 0, sizeof(ST_MAC_HOSTNAME));

    if (NULL == hostName || '\0' == *hostName) {
	hostName = "";
    }

    strcpy(macHostName->hwaddr, macAddr);
    strcpy(macHostName->hostname, hostName);
    macHostName->cfgIndex = cfgIndex;

    return macHostName;
}

ST_PORT_FORWARD *invokeAclPortForwardCreate(char *ruleIdentification, char *ruleName, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort, int cfgIndex)
{
	ST_PORT_FORWARD *portForward;
	portForward = INVOKE_TALLOC(ST_PORT_FORWARD);
	if (portForward == NULL) {
        	CPE_LOG_ERROR("init ST_PORT_FORWARD sturct error, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
	 			ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
        	return NULL;
    	}
	memset(portForward, 0, sizeof(ST_PORT_FORWARD));

	if (sourcePort > 65535 || destinationPort > 65535 || sourcePort * destinationPort == 0) {
		CPE_LOG_ERROR("create ST_PORT_FORWARD sturct error, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
	 			ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
		return NULL;
	}

	if (NULL == ruleIdentification || '\0' == *ruleIdentification) {
		CPE_LOG_ERROR("create ST_PORT_FORWARD sturct error, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
	 			ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
		return NULL;
	}

	if (NULL == protocols || '\0' == *protocols) {
		CPE_LOG_ERROR("create ST_PORT_FORWARD sturct error, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
	 			ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
		return NULL;
	}

	if (NULL == destinationIP || '\0' == *destinationIP) {
		CPE_LOG_ERROR("create ST_PORT_FORWARD sturct error, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
	 			ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
		return NULL;
	}

	if (NULL == ruleName || '\0' == *ruleName) {
		CPE_LOG_ERROR("create ST_PORT_FORWARD sturct error, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
	 			ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
		return NULL;
	}
	
	strcpy(portForward->ruleIdentification, ruleIdentification);
	portForward->sourcePort = sourcePort;
	strcpy(portForward->ruleName, ruleName);
	strcpy(portForward->protocols, protocols);
    	strcpy(portForward->destinationIP, destinationIP);
	portForward->destinationPort = destinationPort;
	portForward->cfgIndex = cfgIndex;

	return portForward;
}


int invokeAclAddMacToList(PST_INVOKE_ACL invokeAcl, const char *macAddr, const char *hostName, int accessMode)
{
    const char *actionStr = NULL;

    if (accessMode == 0) 
    {
        utl_node_t* pn = utl_list_begin(&invokeAcl->blackList);
        while (pn != utl_list_end(&invokeAcl->blackList))
        {
            ST_MAC_HOSTNAME *macHostName = (ST_MAC_HOSTNAME *)pn;
            if (strcasecmp(macHostName->hwaddr, macAddr) == 0) 
            {
                return -11;
            }
            
            pn = utl_list_next(pn);
        }

        int cfgIndex = 0;
        char macHostNameStr[140];
        snprintf(macHostNameStr, sizeof(macHostNameStr), "%s %s", macAddr, hostName);
        utl_cfg_add(invokeAcl->cfg, CPE_CFG_BLACK_LIST, &cfgIndex, 0, macHostNameStr);
        
        ST_MAC_HOSTNAME *macHostName = invokeAclMacHostNameCreate(macAddr, hostName, cfgIndex);
        utl_list_add(&invokeAcl->blackList, &macHostName->node);
        actionStr = "DROP";
 
    } 
    else 
    {
        utl_node_t* pn = utl_list_begin(&invokeAcl->whiteList);
        while (pn != utl_list_end(&invokeAcl->whiteList))
        {
            ST_MAC_HOSTNAME *macHostName = ( ST_MAC_HOSTNAME *)pn;
            if (strcasecmp(macHostName->hwaddr, macAddr) == 0) 
            {
                    return -11;
            }
            
            pn = utl_list_next(pn);
        }

        int cfgIndex = 0;
        char macHostNameStr[140];
        
        snprintf(macHostNameStr, sizeof(macHostNameStr), "%s %s", macAddr, hostName);
        utl_cfg_add(invokeAcl->cfg, CPE_CFG_WHITE_LIST, &cfgIndex, 0, macHostNameStr);

        ST_MAC_HOSTNAME *macHostName = invokeAclMacHostNameCreate(macAddr, hostName, cfgIndex);
        utl_list_add(&invokeAcl->whiteList, &macHostName->node);      

        actionStr = "ACCEPT";
    }

    CPE_CfgSave(invokeAcl->cfg);
    if (invokeAcl->accessSwitch == 1 && invokeAcl->accessMode == accessMode) 
    {
        invokeAclIptablesMacCmd(invokeAcl, "I", macAddr, actionStr);
    }

    return 0;
}

int invokeAclRemoveMacFromList(PST_INVOKE_ACL invokeAcl, const char *macAddr, int accessMode)
{
    bool needDec = false;
    const char *nextRule = NULL;
    if (accessMode == 0) 
    {
        nextRule = "DROP";
        utl_node_t* pn = utl_list_begin(&invokeAcl->blackList);

        int delIndex[100];
        int count = 0;
        while (pn != utl_list_end(&invokeAcl->blackList))
        {
            ST_MAC_HOSTNAME *macHostName = ( ST_MAC_HOSTNAME *)pn;
            pn = utl_list_next(pn);
            if (strcasecmp(macHostName->hwaddr, macAddr) == 0) 
            {
                delIndex[count ++] = macHostName->cfgIndex;
                utl_list_del(&invokeAcl->blackList, &macHostName->node);
            }            
        }

        if (count > 0)
        {
            int i = count - 1;
            for (; i >= 0; i--)
            {
                CPE_LOG_DEBUG("delete black list mac address from cfg, i = %d, index = %d", i , delIndex[i]);
                if (utl_cfg_del(invokeAcl->cfg, CPE_CFG_BLACK_LIST, delIndex[i]))
                {
                    CPE_LOG_DEBUG("delete black list mac address from cfg error, macAddr = %s, index = %d", macAddr, delIndex[i]);
                    continue;
                }
                needDec = true;
            }
        }
    }
    else 
    {
        nextRule = "ACCEPT";
        utl_node_t* pn = utl_list_begin(&invokeAcl->whiteList);

        int delIndex[100];
        int count = 0;
        while (pn != utl_list_end(&invokeAcl->whiteList))
        {
            ST_MAC_HOSTNAME *macHostName = ( ST_MAC_HOSTNAME *)pn;
            pn = utl_list_next(pn);
            if (strcasecmp(macHostName->hwaddr, macAddr) == 0) 
            {
                delIndex[count ++] = macHostName->cfgIndex;
                utl_list_del(&invokeAcl->whiteList, &macHostName->node);
            }           
        }

        if (count > 0)
        {
            int i = count - 1;
            for (; i >= 0; i--)
            {
                CPE_LOG_DEBUG("delete white list mac address from cfg, i = %d, index = %d", i , delIndex[i]);
                if (utl_cfg_del(invokeAcl->cfg, CPE_CFG_WHITE_LIST, delIndex[i]))
                {
                    CPE_LOG_DEBUG("delete white list mac address from cfg error, macAddr = %s, index = %d", macAddr, delIndex[i]);
                    continue;
                }
                needDec = true;
            }
        }
        
    }
    if (needDec) {
        CPE_CfgSave(invokeAcl->cfg);
        if (invokeAcl->accessSwitch == 1 && invokeAcl->accessMode == accessMode) 
        {
            invokeAclIptablesMacCmd(invokeAcl, "D", macAddr, nextRule);
        }
	return 0;
    } else {
	return -11;
    } 
}

void invokeAclGetMacAddrList(PST_INVOKE_ACL invokeAcl, int accessMode, char* str_buf, int str_len) 
{
    int len = 0;
    len += snprintf(str_buf+len, str_len-len, "[");
    if (accessMode == 0) {
        utl_node_t* pn = utl_list_begin(&invokeAcl->blackList);
        while (pn != utl_list_end(&invokeAcl->blackList))
        {
            ST_MAC_HOSTNAME *macHostName = (ST_MAC_HOSTNAME *)pn;
            len += snprintf(str_buf+len, str_len-len, 
                "{\"mac_addr\":\"%s\", \"hostname\":\"%s\"},", 
                macHostName->hwaddr, macHostName->hostname);
            pn = utl_list_next(pn);
            
        }
        
    } else {
        utl_node_t* pn = utl_list_begin(&invokeAcl->whiteList);
        while (pn != utl_list_end(&invokeAcl->whiteList))
        {
            ST_MAC_HOSTNAME *macHostName = (ST_MAC_HOSTNAME *)pn;
            len += snprintf(str_buf+len, str_len-len, 
                "{\"mac_addr\":\"%s\", \"hostname\":\"%s\"},", 
                macHostName->hwaddr, macHostName->hostname);
            pn = utl_list_next(pn);
        }
    }
    if (len >= str_len)
    {
        len = str_len - 1;
    }

    /* to delete the last comma ',' */
    len -= (len > 1);
    
    str_buf[len++] = ']';
    str_buf[len] = '\0';
}

int invokeAclIptablesMacCmd(PST_INVOKE_ACL invokeAcl, const char *operating, const char *macAddr, const char *nextRule)
{
	char cmd[128];
    	memset(cmd, 0, sizeof(cmd));
	if (NULL != macAddr && '\0' != *macAddr) {
		sprintf(cmd, "iptables -t filter -%s %s -i wlan0 -m mac --mac-source %s -j %s 2>&1", operating, invokeAcl->rule, macAddr, nextRule);  
	} else {
		if (strcmp(operating, "F") == 0) {
			sprintf(cmd, "iptables -%s %s 2>&1", operating, invokeAcl->rule);
		} else {
			sprintf(cmd, "iptables -t filter -%s %s -i wlan0 -j %s 2>&1", operating, invokeAcl->rule, nextRule);
		}
	}
	invokeAclSendShellCmd(cmd);
	return 0;
}

int invokeAclAddDnatPortToList(PST_INVOKE_ACL invokeAcl, char *ruleIdentification, char *ruleName, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort)
{
        utl_node_t* pn = utl_list_begin(&invokeAcl->portForwardList);
        while (pn != utl_list_end(&invokeAcl->portForwardList))
        {
            ST_PORT_FORWARD *portForward = (ST_PORT_FORWARD *)pn;
            if (strcasecmp(portForward->ruleIdentification, ruleIdentification) == 0) 
            {
		CPE_LOG_WRAN("dnat port is already in the list");
                return -11;
            }
            
            pn = utl_list_next(pn);
        }

        int cfgIndex = 0;
        char dnatPortStr[140];
        snprintf(dnatPortStr, sizeof(dnatPortStr), "%s %s %d %s %s %d", ruleIdentification, ruleName, sourcePort, protocols, destinationIP, destinationPort);
        utl_cfg_add(invokeAcl->cfg, CPE_CFG_PORT_FORWARD_LIST, &cfgIndex, 0, dnatPortStr);
        
        ST_PORT_FORWARD *portForward = invokeAclPortForwardCreate(ruleIdentification, ruleName, sourcePort, protocols, destinationIP, destinationPort, cfgIndex);
	if (portForward == NULL) {
		CPE_LOG_ERROR("create dnat port struct error");
		return -12;
	}
        utl_list_add(&invokeAcl->portForwardList, &portForward->node);
    	CPE_CfgSave(invokeAcl->cfg);
	invokeAclIptablesPortCmd("I", sourcePort, protocols, destinationIP, destinationPort);

	return 0;
}

int invokeAclRemoveDnatPortFromList(PST_INVOKE_ACL invokeAcl, char *ruleIdentification)
{
        utl_node_t* pn = utl_list_begin(&invokeAcl->portForwardList);

        int count = 0;
        while (pn != utl_list_end(&invokeAcl->portForwardList))
        {
            pn = utl_list_next(pn);
            ST_PORT_FORWARD *portForward = (ST_PORT_FORWARD *)pn;
            if (strcasecmp(portForward->ruleIdentification, ruleIdentification) == 0) 
            {
		CPE_LOG_DEBUG("delete black list mac address from cfg, index = %d", portForward->cfgIndex);
                if (utl_cfg_del(invokeAcl->cfg, CPE_CFG_PORT_FORWARD_LIST, portForward->cfgIndex))
                {
                    	CPE_LOG_DEBUG("delete dnat port list from cfg error, ruleIdentification = %s, protocols = %s, sourcePort = %d, destinationIP = %s, destinationPort = %d, index = %d", 
					ruleIdentification, portForward->protocols, portForward->sourcePort, 
					portForward->destinationIP, portForward->destinationPort, portForward->cfgIndex);
			return -12;
                }

		
		invokeAclIptablesPortCmd("D", portForward->sourcePort, portForward->protocols, portForward->destinationIP, portForward->destinationPort);
                utl_list_del(&invokeAcl->portForwardList, &portForward->node);
		count ++;
            }            
        }

        if (count > 0) {
	    	CPE_CfgSave(invokeAcl->cfg);
		return 0;
        } else {
		return -11;
	}	
}

int invokeAclIptablesPortCmd(const char *operating, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort)
{
	char cmd[128];
    	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "iptables -t nat -%s PREROUTING -d 192.168.0.0/24 -p %s --dport %d -j DNAT --to %s:%d 2>&1",
			 operating, protocols, sourcePort, destinationIP, destinationPort);
	invokeAclSendShellCmd(cmd);
	return 0;
}

int invokeAclGetDnatPortList(PST_INVOKE_ACL invokeAcl, char* str_buf, int str_len)
{
    	int len = 0;
    	len += snprintf(str_buf+len, str_len-len, "[");
        utl_node_t* pn = utl_list_begin(&invokeAcl->portForwardList);
        while (pn != utl_list_end(&invokeAcl->portForwardList))
        {
            ST_PORT_FORWARD *portForward = (ST_PORT_FORWARD *)pn;
            len += snprintf(str_buf+len, str_len-len, 
                "{\"ruleIdentification\":\"%s\",\"ruleName\":\"%s\",\"protocols\":\"%s\",\"sourcePort\":%d,\"destinationIP\":\"%s\",\"destinationPort\":%d},", 
                portForward->ruleIdentification, portForward->ruleName, portForward->protocols, 
		portForward->sourcePort, portForward->destinationIP, portForward->destinationPort);
            pn = utl_list_next(pn);
            
        }
        

    	if (len >= str_len)
    	{
        	len = str_len - 1;
    	}

    	/* to delete the last comma ',' */
    	len -= (len > 1);
    
    	str_buf[len++] = ']';
    	str_buf[len] = '\0';

	return 0;
}

int invokeAclSendShellCmd(char *cmdStr)
{
	char sendJsonStr[1024];
	int sendJsonSize = 0;
	char resultJsonStr[1024];
	memset(sendJsonStr, '\0' , sizeof(sendJsonStr));
	memset(resultJsonStr, '\0' , sizeof(resultJsonStr));
	sendJsonSize = snprintf(sendJsonStr, sizeof(sendJsonStr), "{\"msg\":\"SHELL\", \"data\":\"%s\"}", cmdStr);
	cwaSock_SendData(ANDROID_SOCK_PORT, sendJsonStr, sendJsonSize, resultJsonStr, sizeof(resultJsonStr));
	CPE_LOG_DEBUG("\n send json: %s,\n  result json: %s ", sendJsonStr, resultJsonStr);
	return 0;
}


#ifdef __cplusplus
}
#endif
