#ifndef  __VWA_ACL_H__
#define  __VWA_ACL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include "invoke_config.h"
#include "cwa_sock.h"

#define INVOKE_MALLOC(s)       malloc(s)
#define INVOKE_FREE(p)         do{ if (p) eos_free(p); }while(0)

#define INVOKE_TALLOC(T)       (T*)INVOKE_MALLOC(sizeof(T))

typedef struct invokeAclRootSt* PST_INVOKE_ACL;

/*
*
**/
PST_INVOKE_ACL INVOKE_ACL_Create(PST_CPE_CFG pCfg);

/* 
 * The request is a black and white list management request, and the request sent from 127.0.0.1 returns true, otherwise it returns false
 **/
int INVOKE_ACL_Ctrl_Iptables(PST_INVOKE_ACL invokeAcl, char *type, char *ruleName, int setValue, char *hostName, char *mac, char *resultStr, int reult_len);

int INVOKE_ACL_Ctrl_Port(PST_INVOKE_ACL invokeAcl, char *type, char *ruleIdentification, char *ruleName, unsigned int sourcePort, char *protocols, char *destinationIP, unsigned int destinationPort, char *resultStr, int reult_len);

#ifdef __cplusplus
}
#endif

#endif
