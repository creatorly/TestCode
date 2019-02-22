#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
//#include <json-c/json.h>
#include <time_log.h>
#include <invoke_acl.h>

#include "invoke_server.h"
#include "cwa_sock.h"
#include <cpeLog.h>
//#include "invoke_acl.h"

#define INVOKE_SERVER_VERSION "1.0.3"

static struct ubus_context * ctx = NULL;
static struct blob_buf b;
static const char * sock_path;
static PST_INVOKE_ACL g_invokeAcl = NULL;
 
enum
{
	SESSION_LOGIN_USERNAME,
	SESSION_LOGIN_PASSWORD,
	__SESSION_POLICY_MAX,
};
 
static const struct blobmsg_policy session_policy[__SESSION_POLICY_MAX] = {
	[SESSION_LOGIN_USERNAME] = {.name = "username", .type = BLOBMSG_TYPE_STRING},
	[SESSION_LOGIN_PASSWORD] = {.name = "password", .type = BLOBMSG_TYPE_STRING},
};

static int ubus_base_request(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	blob_buf_init(&b, 0);
	
	char sendJsonStr[1024];
	int sendJsonSize = 0;
	char resultJsonStr[1024];
	memset(sendJsonStr, '\0' , sizeof(sendJsonStr));
	memset(resultJsonStr, '\0' , sizeof(resultJsonStr));
	sendJsonSize = snprintf(sendJsonStr, sizeof(sendJsonStr), "{\"ubus_rpc_session\":\"%s\", \"object\":\"%s\", \"method\":\"%s\", \"data\":%s}", EU_UBUS_DEFAULT_SID, obj->name, method, blobmsg_format_json(msg, true));
	CPE_LOG_DEBUG("ubus_base_request recv msg, sendJson = %s", sendJsonStr);
	cwaSock_SendData(ANDROID_SOCK_PORT, sendJsonStr, sendJsonSize, resultJsonStr, sizeof(resultJsonStr));
	CPE_LOG_DEBUG("object: %s, method: %s, \n send json: %s,\n  result json: %s ", obj->name, method, sendJsonStr, resultJsonStr);
	blobmsg_add_json_from_string(&b, resultJsonStr);
	ubus_send_reply(ctx, req, b.head);
	return 0;
}

/* session 方法列表*/
static struct ubus_method session_methods[] = 
{
	UBUS_METHOD("login", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type session_obj_type
= UBUS_OBJECT_TYPE("session", session_methods);
 
static struct ubus_object session_obj = 
{	
	.name = "session", /* 对象的名字 */
	.type = &session_obj_type,
	.methods = session_methods,
	.n_methods = ARRAY_SIZE(session_methods),

};

/* system 方法列表*/
static struct ubus_method system_methods[] = 
{
	UBUS_METHOD("info", ubus_base_request, session_policy),
	UBUS_METHOD("board", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type system_obj_type
= UBUS_OBJECT_TYPE("session", system_methods);
 
static struct ubus_object system_obj = 
{	
	.name = "system", /* 对象的名字 */
	.type = &system_obj_type,
	.methods = system_methods,
	.n_methods = ARRAY_SIZE(system_methods),

};

/* eu_system 方法列表*/
static struct ubus_method eu_system_methods[] = 
{
	UBUS_METHOD("dmesg", ubus_base_request, session_policy),
	UBUS_METHOD("syslog", ubus_base_request, session_policy),
	UBUS_METHOD("diskfree", ubus_base_request, session_policy),
	UBUS_METHOD("process_list", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type eu_system_obj_type
= UBUS_OBJECT_TYPE("vuci.system", eu_system_methods);
 
static struct ubus_object eu_system_obj = 
{	
	.name = "vuci.system", /* 对象的名字 */
	.type = &eu_system_obj_type,
	.methods = eu_system_methods,
	.n_methods = ARRAY_SIZE(eu_system_methods),

};

/* eu_system 方法列表*/
static struct ubus_method eu_network_methods[] = 
{
	UBUS_METHOD("arp_table", ubus_base_request, session_policy),
	UBUS_METHOD("routes", ubus_base_request, session_policy),
	UBUS_METHOD("routes6", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type eu_network_obj_type
= UBUS_OBJECT_TYPE("vuci.network", eu_network_methods);
 
static struct ubus_object eu_network_obj = 
{	
	.name = "vuci.network", /* 对象的名字 */
	.type = &eu_network_obj_type,
	.methods = eu_network_methods,
	.n_methods = ARRAY_SIZE(eu_network_methods),

};

/* network.wifi */
static struct ubus_method network_wifi_methods[] = 
{
	UBUS_METHOD("wifi_info", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type network_wifi_obj_type
= UBUS_OBJECT_TYPE("network.wifi", network_wifi_methods);
 
static struct ubus_object network_wifi_obj = 
{	
	.name = "network.wifi", /* 对象的名字 */
	.type = &network_wifi_obj_type,
	.methods = network_wifi_methods,
	.n_methods = ARRAY_SIZE(network_wifi_methods),

};

/* network.setting */
static struct ubus_method network_setting_methods[] = 
{
	UBUS_METHOD("network_info", ubus_base_request, session_policy),
	UBUS_METHOD("network_setting", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type network_setting_obj_type
= UBUS_OBJECT_TYPE("network.setting", network_setting_methods);
 
static struct ubus_object network_setting_obj = 
{	
	.name = "network.setting", /* 对象的名字 */
	.type = &network_setting_obj_type,
	.methods = network_setting_methods,
	.n_methods = ARRAY_SIZE(network_setting_methods),

};

// network_interface functions
/* network_interface 方法列表*/
static struct ubus_method network_interface_methods[] = 
{
	UBUS_METHOD("status", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type network_interface_obj_type
= UBUS_OBJECT_TYPE("network.interface", network_interface_methods);
 
static struct ubus_object network_interface_obj = 
{	
	.name = "network.interface", /* 对象的名字 */
	.type = &network_interface_obj_type,
	.methods = network_interface_methods,
	.n_methods = ARRAY_SIZE(network_interface_methods),

};

/* network.security 方法列表 */
enum
{
	NETWORK_SECURITY_TYPE,
	NETWORK_SECURITY_RULE_NAME,
	NETWORK_SECURITY_SET_VALUE,
	NETWORK_SECURITY_HOSTNAME,
	NETWORK_SECURITY_MAC,
	__NETWORK_SECURITY_MAX,
};
 
static const struct blobmsg_policy access_control_policy[__NETWORK_SECURITY_MAX] = {
	[NETWORK_SECURITY_TYPE] 	= {.name = "type", 	.type = BLOBMSG_TYPE_STRING},
	[NETWORK_SECURITY_RULE_NAME] 	= {.name = "ruleName", 	.type = BLOBMSG_TYPE_STRING},
	[NETWORK_SECURITY_SET_VALUE] 	= {.name = "setValue", 	.type = BLOBMSG_TYPE_INT32},
	[NETWORK_SECURITY_HOSTNAME] 	= {.name = "hostName", 	.type = BLOBMSG_TYPE_STRING},
	[NETWORK_SECURITY_MAC] 		= {.name = "mac", 	.type = BLOBMSG_TYPE_STRING},
};

static int ubus_access_control(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	blob_buf_init(&b, 0);
	char resultJsonStr[1024];
	char *type = NULL;
	char *ruleName = NULL;
	int setValue = 0;
	char *hostName = NULL;
	char *mac = NULL;
	
	struct blob_attr *tb[__NETWORK_SECURITY_MAX];
	blobmsg_parse(access_control_policy, __NETWORK_SECURITY_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (tb[NETWORK_SECURITY_TYPE] != NULL) {
		type = blobmsg_get_string(tb[NETWORK_SECURITY_TYPE]);
	}
	
	if (tb[NETWORK_SECURITY_RULE_NAME] != NULL) {
		ruleName = blobmsg_get_string(tb[NETWORK_SECURITY_RULE_NAME]);
	}

	if (tb[NETWORK_SECURITY_SET_VALUE] != NULL) {
		setValue = blobmsg_get_u32(tb[NETWORK_SECURITY_SET_VALUE]);
	}

	if (tb[NETWORK_SECURITY_HOSTNAME] != NULL) {
		hostName = blobmsg_get_string(tb[NETWORK_SECURITY_HOSTNAME]);
	}

	if (tb[NETWORK_SECURITY_MAC] != NULL) {
		mac = blobmsg_get_string(tb[NETWORK_SECURITY_MAC]);
	}

	CPE_LOG_DEBUG("type = %s, ruleName = %s, setValue = %d, hostName = %s, mac = %s", type, ruleName, setValue, hostName, mac);
	INVOKE_ACL_Ctrl_Iptables(g_invokeAcl, type, ruleName, setValue, hostName, mac, resultJsonStr, sizeof(resultJsonStr));
	CPE_LOG_DEBUG("ubus_access_control, resultJsonStr: %s", resultJsonStr);
	blobmsg_add_json_from_string(&b, resultJsonStr);
	ubus_send_reply(ctx, req, b.head);
	
	return 0;
}

static struct ubus_method network_security_methods[] = 
{
	UBUS_METHOD("access_control", ubus_access_control, access_control_policy),
	UBUS_METHOD("phone_connects", ubus_base_request, session_policy),
	UBUS_METHOD("pwd_alter", ubus_base_request, session_policy),
	UBUS_METHOD("page_access", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type network_security_obj_type
= UBUS_OBJECT_TYPE("network.security", network_security_methods);
 
static struct ubus_object network_security_obj = 
{	
	.name = "network.security", /* 对象的名字 */
	.type = &network_security_obj_type,
	.methods = network_security_methods,
	.n_methods = ARRAY_SIZE(network_security_methods),
};

/* network.dhcp */
static struct ubus_method network_dhcp_methods[] = 
{
	UBUS_METHOD("dhcp", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type network_dhcp_obj_type
= UBUS_OBJECT_TYPE("network.dhcp", network_dhcp_methods);
 
static struct ubus_object network_dhcp_obj = 
{	
	.name = "network.dhcp", /* 对象的名字 */
	.type = &network_dhcp_obj_type,
	.methods = network_dhcp_methods,
	.n_methods = ARRAY_SIZE(network_dhcp_methods),
};

/* device.manager */
static struct ubus_method device_manager_methods[] = 
{
	UBUS_METHOD("system_upgrade", ubus_base_request, session_policy),
	UBUS_METHOD("log_upload", ubus_base_request, session_policy),
	UBUS_METHOD("system_reset", ubus_base_request, session_policy),
	UBUS_METHOD("system_backup", ubus_base_request, session_policy),
	UBUS_METHOD("system_restore", ubus_base_request, session_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type device_manager_obj_type
= UBUS_OBJECT_TYPE("device.manager", device_manager_methods);
 
static struct ubus_object device_manager_obj = 
{	
	.name = "device.manager", /* 对象的名字 */
	.type = &device_manager_obj_type,
	.methods = device_manager_methods,
	.n_methods = ARRAY_SIZE(device_manager_methods),
};

/* setting.ports 方法列表 */
enum
{
	PORTS_FORWARDING_TYPE,
	PORTS_FORWARDING_RULE_IDENTIFICATION,
	PORTS_FORWARDING_RULE_NAME,
	PORTS_FORWARDING_SOURCE_PORT,
	PORTS_FORWARDING_PROTOCOLS,
	PORTS_FORWARDING_DESTINATION_IP,
	PORTS_FORWARDING_DESTINATION_PORT,
	__PORTS_FORWARDING_MAX,
};
 
static const struct blobmsg_policy ports_farwarding_policy[__PORTS_FORWARDING_MAX] = {
	[PORTS_FORWARDING_TYPE] 		= {.name = "type", 			.type = BLOBMSG_TYPE_STRING},
	[PORTS_FORWARDING_RULE_IDENTIFICATION] 	= {.name = "ruleIdentification", 	.type = BLOBMSG_TYPE_STRING},
	[PORTS_FORWARDING_RULE_NAME] 		= {.name = "ruleName", 			.type = BLOBMSG_TYPE_STRING},
	[PORTS_FORWARDING_SOURCE_PORT] 		= {.name = "sourcePort", 		.type = BLOBMSG_TYPE_INT32},
	[PORTS_FORWARDING_PROTOCOLS] 		= {.name = "protocols", 		.type = BLOBMSG_TYPE_STRING},
	[PORTS_FORWARDING_DESTINATION_IP] 	= {.name = "destinationIP", 		.type = BLOBMSG_TYPE_STRING},
	[PORTS_FORWARDING_DESTINATION_PORT] 	= {.name = "destinationPort", 		.type = BLOBMSG_TYPE_INT32},
};

static int ubus_ports_farwarding(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	
	blob_buf_init(&b, 0);
	char resultJsonStr[1024];
	char *type = NULL;
	char *ruleIdentification = NULL;
	char *ruleName = NULL;
	int sourcePort = 0;
	char *protocols = NULL;
	char *destinationIP = NULL;
	int destinationPort = 0;
	
	struct blob_attr *tb[__PORTS_FORWARDING_MAX];
	blobmsg_parse(ports_farwarding_policy, __PORTS_FORWARDING_MAX, tb, blob_data(msg), blob_len(msg));
	
	if (tb[PORTS_FORWARDING_TYPE] != NULL) {
		type = blobmsg_get_string(tb[PORTS_FORWARDING_TYPE]);
	}
	
	if (tb[PORTS_FORWARDING_RULE_IDENTIFICATION] != NULL) {
		ruleIdentification = blobmsg_get_string(tb[PORTS_FORWARDING_RULE_IDENTIFICATION]);
	}

	if (tb[PORTS_FORWARDING_RULE_NAME] != NULL) {
		ruleName = blobmsg_get_string(tb[PORTS_FORWARDING_RULE_NAME]);
	}

	if (tb[PORTS_FORWARDING_SOURCE_PORT] != NULL) {
		sourcePort = blobmsg_get_u32(tb[PORTS_FORWARDING_SOURCE_PORT]);
	}

	if (tb[PORTS_FORWARDING_PROTOCOLS] != NULL) {
		protocols = blobmsg_get_string(tb[PORTS_FORWARDING_PROTOCOLS]);
	}

	if (tb[PORTS_FORWARDING_DESTINATION_IP] != NULL) {
		destinationIP = blobmsg_get_string(tb[PORTS_FORWARDING_DESTINATION_IP]);
	}

	if (tb[PORTS_FORWARDING_DESTINATION_PORT] != NULL) {
		destinationPort = blobmsg_get_u32(tb[PORTS_FORWARDING_DESTINATION_PORT]);
	}

	CPE_LOG_DEBUG("type = %s, ruleIdentification = %s, sourcePort = %d, protocols = %s, destinationIP = %s, destinationPort = %d",
			 type, ruleIdentification, sourcePort, protocols, destinationIP, destinationPort);
	INVOKE_ACL_Ctrl_Port(g_invokeAcl, type, ruleIdentification, ruleName, sourcePort, protocols, destinationIP, destinationPort, resultJsonStr, sizeof(resultJsonStr));
	CPE_LOG_DEBUG("ubus_access_control, resultJsonStr: %s", resultJsonStr);
	blobmsg_add_json_from_string(&b, resultJsonStr);
	ubus_send_reply(ctx, req, b.head);
	
	return 0;

	return 0;
}

static struct ubus_method setting_ports_methods[] = 
{
	UBUS_METHOD("ports_farwarding", ubus_ports_farwarding, ports_farwarding_policy),
};
  
/* type目前没有实际用处 */
static struct ubus_object_type setting_ports_obj_type
= UBUS_OBJECT_TYPE("setting.ports", network_security_methods);
 
static struct ubus_object setting_ports_obj = 
{	
	.name = "setting.ports", /* 对象的名字 */
	.type = &setting_ports_obj_type,
	.methods = setting_ports_methods,
	.n_methods = ARRAY_SIZE(setting_ports_methods),

};

static void ubus_add_fd(void)
{
	ubus_add_uloop(ctx);
 
#ifdef FD_CLOEXEC
	fcntl(ctx->sock.fd, F_SETFD,
		fcntl(ctx->sock.fd, F_GETFD) | FD_CLOEXEC);
#endif
}
 
static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
	static struct uloop_timeout retry =
	{
		.cb = ubus_reconn_timer,
	};
	int t = 2;
 
	if (ubus_reconnect(ctx, sock_path) != 0) {
		uloop_timeout_set(&retry, t * 1000);
		return;
	}
 
	ubus_add_fd();
}
 
static void ubus_connection_lost(struct ubus_context *ctx)
{
	ubus_reconn_timer(NULL);
}
 
static int display_ubus_init(const char *path)
{
	uloop_init();
	sock_path = path;
 
	ctx = ubus_connect(path);
	if (!ctx)
	{
		CPE_LOG_ERROR("ubus connect failed");
		return -1;
	}
	
	CPE_LOG_DEBUG("connected as %08x", ctx->local_id);
	ctx->connection_lost = ubus_connection_lost;
 
	ubus_add_fd();

	CPE_LOG_DEBUG("test log = %s\n", current_time);
	CPE_LOG_DEBUG("start add object, ctx = %p, session_obj = %p", ctx, &session_obj);
	if (ubus_add_object(ctx, &session_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add session obj failed");
		return -1;
	}
  /*
	if (ubus_add_object(ctx, &system_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add system obj failed");
		return -1;
	}
 
	if (ubus_add_object(ctx, &eu_system_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add eu system obj failed");
		return -1;
	}

	if (ubus_add_object(ctx, &network_interface_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add network interface obj failed");
		return -1;
	}

	if (ubus_add_object(ctx, &eu_network_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add eu network obj failed");
		return -1;
	} 

	if (ubus_add_object(ctx, &network_wifi_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add network wifi obj failed");
		return -1;
	}

	if (ubus_add_object(ctx, &network_setting_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add network setting obj failed");
		return -1;
	}

	if (ubus_add_object(ctx, &network_dhcp_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add network dhcp obj failed");
		return -1;
	}

	if (ubus_add_object(ctx, &network_security_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add eu network security obj failed");
		return -1;
	}

	if (ubus_add_object(ctx, &setting_ports_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add eu setting ports obj failed");
		return -1;
	} 

	if (ubus_add_object(ctx, &device_manager_obj) != 0)
	{
		CPE_LOG_ERROR("ubus add device manager obj failed");
		return -1;
	}
*/
	return 0;
}
 
static void display_ubus_done(void)
{
	if (ctx)
		ubus_free(ctx);
}

static int incoke_acl_init(void)
{
	PST_CPE_CFG pCfg;

    	pCfg = CPE_CfgInit(NULL);

    	/* initialize a config handle */
    	if (NULL == pCfg) 
    	{
        	CPE_LOG_ERROR("failed to init cpe config!");
        	return -1;
    	}
	CPE_LOG_DEBUG("cfg init succeed : pcfg = %p", pCfg);
	CPE_CfgSave(pCfg);

	g_invokeAcl = INVOKE_ACL_Create(pCfg);
    	if (NULL == g_invokeAcl) {
        	CPE_LOG_ERROR("failed to init invoke access control!");
        	return -1;
    	}

    	return 0;
}
 
int main(int argc, char * argv[])
{
	char * path = NULL;
	
	CPE_LOG_DEBUG("===========================invoke_server start===========================");
	CPE_LOG_DEBUG("build time:%s %s, version:%s", __DATE__, __TIME__, INVOKE_SERVER_VERSION);

	if (-1 == display_ubus_init(path))
	{
		CPE_LOG_ERROR("ubus connect failed!");
		return -1;
	}

	if (-1 == incoke_acl_init())
	{
		CPE_LOG_ERROR("invoke acl init failed!");
		return -1;
	}
	
	uloop_run();
 
	display_ubus_done();
 
	return 0;
}

