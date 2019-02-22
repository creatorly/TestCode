#ifndef  __VWA_ACL__H__
#define  __VWA_ACL__H__

#include <utl_httpd.h>

#include <utl_list.h>

#ifdef __cplusplus
extern "C" {
#endif



typedef struct invokeAclMacHostName
{
    utl_node_t node;
    int cfgIndex;
    char hwaddr[18];
    char hostname[128];
}ST_MAC_HOSTNAME;

/* Rule—identification Source-Port Protocols Destination-IP Destination-Port*/
typedef struct invokeAclPort
{
    	utl_node_t node;
    	int cfgIndex;
	char ruleIdentification[32];
	char ruleName[128];
	unsigned int sourcePort;
	char protocols[10];
	char destinationIP[16];
	unsigned int destinationPort;
}ST_PORT_FORWARD;

typedef struct invokeAclRootSt
{
    int accessSwitch;		/* 0, don't use, 1: use*/
    int accessMode;             /* 0,black 1:white*/
    utl_list_t blackList;
    utl_list_t whiteList;
    utl_list_t portForwardList;

    const char *rule;
    PST_CPE_CFG cfg;
}ST_INVOKE_ACL;

ST_MAC_HOSTNAME *invokeAclMacHostNameCreate(const char* macAddr, const char *hostName, int cfgIndex);
int invokeAclIptablesMacCmd(PST_INVOKE_ACL vwaAcl, const char *operating, const char *macAddr, const char *nextRule);
int invokeAclSendShellCmd(char *cmdStr);

int invokeAclAddMacToList(PST_INVOKE_ACL invokeAcl, const char *macAddr, const char *hostName, int accessMode);
int invokeAclRemoveMacFromList(PST_INVOKE_ACL invokeAcl, const char *macAddr, int accessMode);
void invokeAclGetMacAddrList(PST_INVOKE_ACL invokeAcl, int accessMode, char* str_buf, int str_len);
void invokeAclchangeIptableMode(PST_INVOKE_ACL invokeAcl, int accessMode);

ST_PORT_FORWARD *invokeAclPortForwardCreate(char *ruleIdentification, char *ruleName, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort, int cfgIndex);
int invokeAclAddDnatPortToList(PST_INVOKE_ACL invokeAcl, char *ruleIdentification, char *ruleName, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort);
int invokeAclRemoveDnatPortFromList(PST_INVOKE_ACL invokeAcl, char *ruleIdentification);
int invokeAclGetDnatPortList(PST_INVOKE_ACL invokeAcl, char* str_buf, int str_len);
int invokeAclIptablesPortCmd(const char *operating, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort);

#ifdef __cplusplus
}
#endif

#endif
